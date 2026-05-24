#include "purple_ball.hpp"
#include "chippie/chippie_common.hpp"
#include "chippie/entities/entity_types.hpp"
#include "chippie/state/terrain_effects.hpp"

namespace chippie::purple_ball
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

constexpr uint64_t PURPLE_BALL_VELOCITY_US{1'000'000}; /* time is in us */

[[nodiscard]] std::pair<Grid::Location, direction> compute_next_location(const entity &ent) noexcept
{
    auto nextloc{ent.loc};

    switch (ent.facing)
    {
    case direction::UP:
        ++nextloc.y;
        break;
    case direction::DOWN:
        --nextloc.y;
        break;
    case direction::RIGHT:
        ++nextloc.x;
        break;
    case direction::LEFT:
        --nextloc.x;
        break;
    }

    return std::make_pair(nextloc, ent.facing);
}

[[nodiscard]] collision_action handle_collision(entity &ent, entity *collided_entity,
                                                terrain_type &collided_terrain) noexcept
{
    /* in the case of the purple ball:
            if it's clear, allow the move,
            if it's the player, game over,
            otherwise, reverse facing and move again
    */
    if (collided_entity != nullptr)
    {
        if (collided_entity->identity == entity_type::CHIPPIE)
        {
            register_event(event::event_type::GAME_OVER);
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
[[nodiscard]] entity create(Grid::Location xy, direction dir, uint8_t uuid) noexcept
{
    return {
        .loc = xy,
        .identity = entity_type::PURPLE_BALL,
        .facing = dir,
        .alive = true,
        .trapped = false,
        .uuid = uuid,
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
    return PURPLE_BALL_VELOCITY_US;
}

} // namespace chippie::purple_ball