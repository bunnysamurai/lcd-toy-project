#include "chippie_entity.hpp"
#include "chippie/chippie_common.hpp"
#include "chippie/entities/basic_entity.hpp"
#include "chippie/entities/entity_types.hpp"
#include "chippie/events/event.hpp"
#include "chippie/state/chippie_inventory.hpp"
#include "chippie/state/terrain_effects.hpp"
#include "chippie/state/terrain_types.hpp"
#include "gamepad/gamepad.hpp"

#include "pico/time.h"
#include <cstddef>
#include <pico/types.h>

#define DEBUG_PRINT
#ifdef DEBUG_PRINT
#include <pico/printf.h>
#endif

namespace chippie::chippie
{
/*
 ____       _            _
|  _ \ _ __(_)_   ____ _| |_ ___
| |_) | '__| \ \ / / _` | __/ _ \
|  __/| |  | |\ V / (_| | ||  __/
|_|   |_|  |_| \_/ \__,_|\__\___|

*/
namespace
{
constexpr uint64_t CHIPPIE_BUTTON_POLL_US{10'000};                   /* every 10 ms? */
constexpr uint64_t CHIPPIE_HOLD_TIME_AFTER_BUTTON_PRESS_US{100'000}; /* every 100 ms? */
bool wait_for_release;
absolute_time_t hold_time_point;

[[nodiscard]] std::pair<Grid::Location, direction> compute_next_location(const entity &ent) noexcept
{
    const auto current_time{get_absolute_time()};

    /* hold any new moves until a timer has expired */
    if (absolute_time_diff_us(current_time, hold_time_point) > 0)
    {
        return std::make_pair(ent.loc, ent.facing);
    }

    hold_time_point = delayed_by_us(hold_time_point, CHIPPIE_HOLD_TIME_AFTER_BUTTON_PRESS_US);

    const gamepad::five::State buttons{gamepad::five::get()};

    if (wait_for_release)
    {
        wait_for_release = buttons.etc || buttons.up || buttons.down || buttons.left || buttons.right;
        return std::make_pair(ent.loc, ent.facing);
    }

    if (buttons.etc)
    {
        wait_for_release = true;
        event::register_event(event::event_type::OPEN_MENU);
        return std::make_pair(ent.loc, ent.facing);
    }

    if (buttons.up)
    {
        wait_for_release = true;
        return std::make_pair(Grid::Location{.x = ent.loc.x, .y = ent.loc.y - 1}, direction::UP);
    }
    if (buttons.down)
    {
        wait_for_release = true;
        return std::make_pair(Grid::Location{.x = ent.loc.x, .y = ent.loc.y + 1}, direction::DOWN);
    }
    if (buttons.right)
    {
        wait_for_release = true;
        return std::make_pair(Grid::Location{.x = ent.loc.x + 1, .y = ent.loc.y}, direction::RIGHT);
    }
    if (buttons.left)
    {
        wait_for_release = true;
        return std::make_pair(Grid::Location{.x = ent.loc.x - 1, .y = ent.loc.y}, direction::LEFT);
    }

    /* otherwise, change facing to down */
    return std::make_pair(ent.loc, direction::DOWN);
}

[[nodiscard]] collision_action handle_door(inventory &invent, terrain_type collided_terrain) noexcept
{
    if (collided_terrain == terrain_type::GREEN_DOOR && invent.check(inventory_item::GREEN_KEY) > 0)
    {
        return collision_action::APPLY_NEXT_LOCATION;
    }
    if (collided_terrain == terrain_type::YELLOW_DOOR && invent.check(inventory_item::YELLOW_KEY) > 0)
    {
        invent.remove(inventory_item::YELLOW_KEY);
        return collision_action::APPLY_NEXT_LOCATION;
    }
    if (collided_terrain == terrain_type::RED_DOOR && invent.check(inventory_item::RED_KEY) > 0)
    {
        invent.remove(inventory_item::RED_KEY);
        return collision_action::APPLY_NEXT_LOCATION;
    }
    if (collided_terrain == terrain_type::CYAN_DOOR && invent.check(inventory_item::CYAN_KEY) > 0)
    {
        invent.remove(inventory_item::CYAN_KEY);
        return collision_action::APPLY_NEXT_LOCATION;
    }
    return collision_action::NO_ACTION_NEEDED;
}

[[nodiscard]] collision_action handle_socket(inventory &invent) noexcept
{
    if (invent.check(inventory_item::CHIPS) == 0)
    {
        return collision_action::APPLY_NEXT_LOCATION;
    }
    return collision_action::NO_ACTION_NEEDED;
}

[[nodiscard]] collision_action handle_movable_block(entity &ent) noexcept
{
    /* using the entity's facing and current location, we can infer where the movable block needs to go
        we then query the static map if the new location for the block is allowed or not
        if it is, we update the static map and proceed with the next move
        otherwise, we say no action is needed.
     */

    const auto current_location_of_movable_block{ent.loc};

    const auto candidate_location_for_moveable_block{move(current_location_of_movable_block, ent.facing)};

    const terrain_type candidate_terrain{ent.game_state->the_map[candidate_location_for_moveable_block]};

    /* what are the rules, here?
            If it's water, turn it into dirt
            If it's clear, allow the move
    */
    if (check_terrain_is_opaque_for_moveable(ent.facing, candidate_terrain))
    {
        return collision_action::NO_ACTION_NEEDED;
    }

    /* collided location clears it's moveable status */
    ent.game_state->the_map[current_location_of_movable_block].clear_moveable();

    /* candidate location now has a moveable block on top, unless it landed in water, in which case it becomes dirt */
    if (candidate_terrain == terrain_type::WATER)
    {
        ent.game_state->the_map[candidate_location_for_moveable_block] = terrain_type::DIRT;
    }
    else
    {
        ent.game_state->the_map[candidate_location_for_moveable_block].set_moveable();
    }

    return collision_action::APPLY_NEXT_LOCATION;
}

[[nodiscard]] collision_action handle_dirt(entity &ent) noexcept
{
    ent.game_state->the_map[ent.loc] = terrain_type::CLEAR;
    return collision_action::APPLY_NEXT_LOCATION;
}

[[nodiscard]] collision_action handle_collision(entity &ent, entity *collided_entity,
                                                terrain_type collided_terrain) noexcept
{
    /* for level 1 */
    switch (collided_terrain)
    {
    case terrain_type::GREEN_DOOR:
    case terrain_type::CYAN_DOOR:
    case terrain_type::YELLOW_DOOR:
    case terrain_type::RED_DOOR:
        return handle_door(ent.game_state->chippie_inventory, collided_terrain);
    case terrain_type::WALL:
        return collision_action::NO_ACTION_NEEDED;
    case terrain_type::SOCKET:
        return handle_socket(ent.game_state->chippie_inventory);
    case terrain_type::WATER:
        event::register_event(event::event_type::GAME_OVER);
        return collision_action::APPLY_NEXT_LOCATION;
    case terrain_type::MOVABLE_BLOCK:
        return handle_movable_block(ent);
    case terrain_type::DIRT:
        return handle_dirt(ent);
    default:
        break;
    }

    return collision_action::APPLY_NEXT_LOCATION;
}

} // namespace

/*
 ____        _     _ _
|  _ \ _   _| |__ | (_) ___
| |_) | | | | '_ \| | |/ __|
|  __/| |_| | |_) | | | (__
|_|    \__,_|_.__/|_|_|\___|

*/
entity create(state &game_state, Grid::Location xy, direction dir, uint8_t uuid) noexcept
{
    hold_time_point = get_absolute_time();
    wait_for_release = true; /* just in case the button is still being pressed when a level loads */

    return {
        .loc = xy,
        .identity = entity_type::CHIPPIE,
        .facing = dir,
        .next_time = hold_time_point,
        .alive = true,
        .trapped = false,
        .uuid = uuid,
        .game_state = &game_state,
    };
}

entity_state_machine get_state_functions() noexcept
{
    return {.entry_handler = nullptr,
            .process_move_handler = compute_next_location,
            .entity_collision_handler = handle_collision,
            .exit_handler = nullptr};
}

/**
    @brief Retrieve velocity for moves of Chippie.  For a player-controlled entity, this is effectively the poll rate on
   the button presses.  Gating new moves between presses is done by other mechanisms private to this module.

    @return Period between individual moves, in microseconds.
 */
[[nodiscard]] uint64_t get_velocity() noexcept
{
    return CHIPPIE_BUTTON_POLL_US;
}

} // namespace chippie::chippie
