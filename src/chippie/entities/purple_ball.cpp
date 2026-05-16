#include "purple_ball.hpp"
#include "chippie/collision/collision.hpp"
#include "chippie/entities/entity_types.hpp"
#include "chippie_common.hpp"

namespace chippie
{

namespace
{

constexpr uint16_t PURPLE_BALL_VELOCITY{100}; /* TODO totally made up */

} // namespace

[[nodiscard]] Grid::Location compute_next_location(entity ent) noexcept
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

    return nextloc;
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
        switch (collided_entity->identity)
        {
        case entity_type::CHIPPIE:
            register_event(event::event_type::GAME_OVER);
            break;
        case entity_type::WALL:
            return collision_action::MOVE_BACKWARD;
        default:
            return collision_action::MOVE_FORWARD;
        }
    }

    apply_terrain_effect(ent, collided_terrain);
}

[[nodiscard]] entity create(Grid::Location xy, direction dir) noexcept
{
    return entity{.identity = entity_type::PURPLE_BALL,
                  .alive = true,
                  .facing = dir,
                  .loc = xy,
                  .velocity_ticks = PURPLE_BALL_VELOCITY};
}
} // namespace chippie