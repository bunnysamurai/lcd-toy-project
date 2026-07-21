#include "cyan_stick_ball.hpp"
#include "chippie/chippie_common.hpp"
#include "chippie/entities/entity_types.hpp"
#include "chippie/state/terrain_effects.hpp"

#include "common/rng.hpp"

namespace chippie::cyan_stick_ball
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

constexpr uint64_t CYAN_STICK_BALL_VELOCITY_US{250'000}; /* time is in us */

[[nodiscard]] direction handle_facing_change(direction facing) noexcept
{
    /*
        the cyan ball will randomly decide to either move left/right or up/down
        when it hits a wall.

    */
    const auto val{rng::prng()};
    const bool do_a_turn{(val & 0b1) == 0};
    const bool prefer_clockwise{(val & 0b10) != 0};

    if (do_a_turn)
    {
        return prefer_clockwise ? rotate_clockwise(facing) : rotate_anticlockwise(facing);
    }
    return facing;
}

[[nodiscard]] std::pair<Grid::Location, direction> compute_next_location(const entity &ent) noexcept
{
    const auto candidate_location{move(ent.loc, ent.facing)};
    const auto next_terrain{ent.game_state->the_map[candidate_location]};
    if (check_terrain_is_opaque(ent, next_terrain))
    {
        const auto new_facing = handle_facing_change(ent.facing);
        return std::make_pair(move(ent.loc, new_facing), new_facing);
    }
    return std::make_pair(candidate_location, ent.facing);
}

[[nodiscard]] collision_action handle_collision(entity &ent, entity *collided_entity,
                                                terrain_type collided_terrain) noexcept
{
    /* in the case of the cyan_stick ball:
            if it's clear, allow the move,
            if it's the player, game over,
            otherwise, reverse facing and move again
    */
    if (collided_entity != nullptr)
    {
        if (collided_entity->identity == entity_type::CHIPPIE)
        {
            register_event(event::event_type::ROLLED_BY_BALL);
        }
        else
        {
            return collision_action::MOVE_BACKWARD;
        }
    }

    if (!check_terrain_is_opaque(ent, collided_terrain))
    {
        return collision_action::MOVE_FORWARD;
    }
    else
    {
        return collision_action::MOVE_BACKWARD;
    }
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
        .identity = entity_type::CYAN_STICK_BALL,
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
    return CYAN_STICK_BALL_VELOCITY_US;
}

} // namespace chippie::cyan_stick_ball