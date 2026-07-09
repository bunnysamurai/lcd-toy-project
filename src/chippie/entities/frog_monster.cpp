#include "frog_monster.hpp"
#include "chippie/chippie_common.hpp"
#include "chippie/entities/entity_types.hpp"
#include "chippie/state/terrain_effects.hpp"

#include <array>

// #define DEBUG_PRINT
#ifdef DEBUG_PRINT
#include "pico/printf.h"
#endif

namespace chippie::frog_monster
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

constexpr uint64_t FROG_MONSTER_VELOCITY_US{750'000}; /* time is in us */

[[nodiscard]] bool check_terrain_is_opaque_for_frog_monster(const entity &ent, terrain_type candidate_terrain) noexcept
{
    return check_terrain_is_opaque(ent, candidate_terrain);
}

[[nodiscard]] direction to_direction_x(int amount)
{
    if (amount < 0)
    {
        return direction::LEFT;
    }
    return direction::RIGHT;
}

[[nodiscard]] direction to_direction_y(int amount)
{
    if (amount < 0)
    {
        return direction::UP;
    }
    return direction::DOWN;
}

/**
    @return Amount to move in x-direction and y-direction
 */
[[nodiscard]] std::pair<int, int> compute_candidate_direction(Grid::Location monster_loc,
                                                              const state &game_state) noexcept
{
    /*
        frog_monster movement a bit smarter: we path straight towards chippie

        How do we do this?  By decreasing the largest magnitude of distance.

        Let's think this out a bit...

        assume:
            cxy - chippie's xy location
            fxy - frog man's xy location

        Frogman has a decision to make: which 4-adjacent square to move to?
        Frogman should move "as fast as possible"
        I interpret this to mean "decrease the axis of movement that is the largest"
        We can do this by subtracting chippie's location from frogman's:
            cxy - fxy = [dx, dy]
        If |dx| > |dy|, then
            if dx < 0, move left
            else move right
        else
            if dy < 0, move up
            else move down
    */

    const auto [chipx, chipy]{game_state.entity_list.front().loc};
    const int32_t dx{static_cast<int32_t>(chipx) - monster_loc.x};
    const int32_t dy{static_cast<int32_t>(chipy) - monster_loc.y};

#ifdef DEBUG_PRINT
    printf("chipx, chipy: { %u, %u }\n", chipx, chipy);
    printf("dx, dy: { %d, %d }\n", dx, dy);
#endif

    const int xmove{dx == 0 ? 0 : (dx < 0 ? -1 : 1)};
    const int ymove{dy == 0 ? 0 : (dy < 0 ? -1 : 1)};

    return std::make_pair(xmove, ymove);
}

[[nodiscard]] std::pair<Grid::Location, direction> compute_next_location(const entity &ent) noexcept
{
    /* helper method, which will determine if a move should be tried at all, and if so, compute the candidate location
     */
    auto &&make_attempt{[=](const int amount, bool is_x_direction) {
        if (amount == 0)
        {
            return std::make_tuple(false, Grid::Location{}, direction::UP);
        }
        const direction cand_dir{is_x_direction ? to_direction_x(amount) : to_direction_y(amount)};
        const auto candidate_location{move(ent.loc, cand_dir)};
        const auto candidate_terrain{ent.game_state->the_map[candidate_location]};
        if (!check_terrain_is_opaque_for_frog_monster(ent, candidate_terrain))
        {
            return std::make_tuple(true, candidate_location, cand_dir);
        }
        return std::make_tuple(false, Grid::Location{}, direction::UP);
    }};

    const auto [xamount, yamount]{compute_candidate_direction(ent.loc, *ent.game_state)};
#ifdef DEBUG_PRINT
    printf("xamount, yamount: { %d, %d }\n", xamount, yamount);
#endif

    /* if randval is set, try x first, then y */
    const auto randval{(ent.loc.x + 1) * (ent.loc.y + 1) & 0b1};
    const auto first_move{randval ? xamount : yamount};
    const auto second_move{randval ? yamount : xamount};
    {
        const auto [try_move, candidate_location, cand_dir]{make_attempt(first_move, randval)};
        if (try_move)
        {
#ifdef DEBUG_PRINT
            printf("trying the first move, which is %s\n", randval ? "xdirection" : "ydirection");
#endif
            return std::make_pair(candidate_location, cand_dir);
        }
    }
    {
        const auto [try_move, candidate_location, cand_dir]{make_attempt(second_move, !randval)};
        if (try_move)
        {
#ifdef DEBUG_PRINT
            printf("trying the second move, which is %s\n", randval ? "xdirection" : "ydirection");
#endif
            return std::make_pair(candidate_location, cand_dir);
        }
    }

    /* made it here?  Then don't move */
#ifdef DEBUG_PRINT
    printf("not trying a move\n");
#endif
    return std::make_pair(ent.loc, ent.facing);
}

[[nodiscard]] collision_action handle_collision(entity &ent, entity *collided_entity,
                                                terrain_type collided_terrain) noexcept
{
#ifdef DEBUG_PRINT
    printf("frog_monster collision handling start\n");
#endif
    if (collided_entity != nullptr)
    {
        if (collided_entity->identity == entity_type::CHIPPIE)
        {
#ifdef DEBUG_PRINT
            printf("collided with chip\n");
#endif
            register_event(event::event_type::CHOMPED_BY_FROG);
        }
        else
        {
            return collision_action::NO_ACTION_NEEDED;
        }
    }

#ifdef DEBUG_PRINT
    printf("frog_monster collision handling end\n");
#endif

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
[[nodiscard]] entity create(state &game_state, Grid::Location xy, direction dir, uint8_t uuid) noexcept
{
    return {
        .loc = xy,
        .identity = entity_type::FROG_MONSTER,
        .facing = dir,
        .alive = true,
        .trapped = false,
        .uuid = uuid,
        .game_state = &game_state,
    };
}

entity_state_machine get_state_functions() noexcept
{
    return {
        .entry_handler = nullptr,
        .process_move_handler = compute_next_location,
        .entity_collision_handler = handle_collision,
        .exit_handler = nullptr,
    };
}

[[nodiscard]] uint64_t get_velocity() noexcept
{
    return FROG_MONSTER_VELOCITY_US;
}

} // namespace chippie::frog_monster