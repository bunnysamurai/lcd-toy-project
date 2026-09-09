#include "game_logic.hpp"

#include "state.hpp"
#include "terrain_effects.hpp"

#include "chippie/events/event.hpp"

#include <algorithm>
#include <utility>

#include <pico/time.h>

#define DEBUG_PRINT
#ifdef DEBUG_PRINT
#include <pico/printf.h>
#endif

namespace chippie
{

game_logic::game_logic(state &injected_state) noexcept : game_state{injected_state}
{
    const auto game_start_time{injected_state.the_clock.now()};

    /* synchronize all entities in time */
    for (auto &ent : game_state.entity_list)
    {
        ent.next_time =
            injected_state.the_clock.increment_time_point(game_start_time, get_entity_velocity(ent.identity));
    }
}

bool game_logic::is_active() noexcept
{
    return game_state.active;
}

void game_logic::process() noexcept
{
    process_time_remaining();

    move_entities();

    /* TODO event processing, which may change the size of entity_list as well as make changes to the_map */
    event::process_events(game_state);

    /* entity cleanup */
    auto first_dead_itr{std::partition(std::begin(game_state.entity_list), std::end(game_state.entity_list),
                                       [](const auto &ent) { return ent.alive; })};
    game_state.entity_list.resize(std::distance(std::begin(game_state.entity_list), first_dead_itr));
}

void game_logic::move_entities() noexcept
{
    for (uint32_t ii = 0;; ++ii)
    {
        if (ii >= std::size(game_state.entity_list))
        {
            break;
        }

        auto &ent{game_state.entity_list[ii]};

        /* get the function table for this entity */
        const auto entity_handles{get_state_functions(ent.identity)};

        /*  If the timer hasn't expired yet for a move, then skip.
         */
        const auto current_time{game_state.the_clock.now()};
        if (game_state.the_clock.time_diff(current_time, ent.next_time) > 0)
        {
            continue;
        }

        /* begin processing with the entry handler */
        if (entity_handles.entry_handler != nullptr)
        {
            entity_handles.entry_handler(ent);
        }

        /*  If this guy is dead, then skip.  Dead entities are cleaned up later.
         */
        if (!ent.alive)
        {
            continue;
        }

        /* If we make it here, we are good to process a move.  Update next move time w/o drift */
        ent.next_time = game_state.the_clock.increment_time_point(ent.next_time, get_entity_velocity(ent.identity));

        /* next, compute where this entity wants to move */
        auto [nextloc, nextfacing]{entity_handles.process_move_handler != nullptr
                                       ? entity_handles.process_move_handler(ent)
                                       : std::make_pair(ent.loc, ent.facing)};

        /* after the entity has its say about where to move, we now allow the terrain to possibly override it */
        apply_terrain_override_effect(ent, nextloc, nextfacing, game_state.the_map[ent.loc]);

        /* At least for now, we always update the facing, even if we don't end up moving.
           note that the collision handler may further edit the facing. */
        ent.facing = nextfacing;

        /* then, determine and resolve collisions */
        const collision_result collision{find_collisions(ent, nextloc)};

        const collision_action action{
            entity_handles.entity_collision_handler != nullptr
                ? entity_handles.entity_collision_handler(ent, collision.other, collision.tile)
                : collision_action::NO_ACTION_NEEDED};

        /* resolve side-effects from collision handler code */
        if (action == collision_action::NO_ACTION_NEEDED)
        {
            continue;
        }

        if (action == collision_action::MARK_DEAD)
        {
            ent.alive = false;
            continue;
        }

        /* if the next location is the same as the current, then there's nothing more to do */
        if (nextloc == ent.loc)
        {
            continue;
        }

        /* here we'll check if the entity is trapped.  Trapped entities can change facing but not move. */
        if (ent.trapped)
        {
            continue;
        }

        /* if we get here, the entity WILL be moving.  We haven't yet applied the move, so let's process the terrain's
         * exit condition if any */
        apply_terrain_exit_effect(ent, game_state.the_map[ent.loc]);

        if (action == collision_action::APPLY_NEXT_LOCATION)
        {
            ent.loc = nextloc;
        }
        else
        {
            const auto direction_to_move{[&]() {
                if (action == collision_action::MOVE_FORWARD)
                {
                    return relative_direction::FORWARD;
                }
                if (action == collision_action::MOVE_BACKWARD)
                {
                    return relative_direction::BACKWARD;
                }
                if (action == collision_action::MOVE_RIGHT)
                {
                    return relative_direction::RIGHT;
                }
                if (action == collision_action::MOVE_LEFT)
                {
                    return relative_direction::LEFT;
                }
                return relative_direction::FORWARD;
            }()};

            move_relative_direction(ent, direction_to_move);
        }

        /* if the entity moved off the map, it's dead */
        if (game_state.play_grid.out_of_bounds(nextloc))
        {
            ent.alive = false;
            continue;
        }

        apply_terrain_entry_effect(ent, game_state.the_map[ent.loc]);

        /* finally, process the entity's exit handler */
        if (entity_handles.exit_handler != nullptr)
        {
            entity_handles.exit_handler(ent);
        }
    }
}

[[nodiscard]] collision_result game_logic::find_collisions(const entity &current_processing_ent,
                                                           Grid::Location next_location) noexcept
{
    collision_result result{.other = nullptr, .tile = game_state.the_map[next_location]};

    /* search for other entities */
    auto entitr =
        std::find_if(std::begin(game_state.entity_list), std::end(game_state.entity_list), [&](const auto &other) {
            return (other.loc == next_location) && (current_processing_ent.uuid != other.uuid);
        });

    if (entitr != std::end(game_state.entity_list))
    {
        result.other = entitr;
    }

    return result;
}

void game_logic::process_time_remaining() noexcept
{
    if (game_state.time_remaining != NO_TIME_LIMIT && game_state.countdown_timer.elapsed())
    {
        game_state.countdown_timer.increment();

        if (game_state.time_remaining == 0)
        {
            event::register_event(event::event_type::TIME_UP);
            return;
        }

        --game_state.time_remaining;
    }
}

} // namespace chippie