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

collision_action handle_collision(const entity &ent, collision_status collision) noexcept
{
    /* in the case of the purple ball:
            if it's clear, allow the move,
            if it's the player, game over,
            otherwise, reverse facing and move again
    */
    switch (collision.type)
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

[[nodiscard]] entity create(Grid::Location xy, direction dir) noexcept
{
    return entity{.identity = entity_type::PURPLE_BALL,
                  .alive = true,
                  .facing = dir,
                  .loc = xy,
                  .velocity_ticks = PURPLE_BALL_VELOCITY};
}
} // namespace chippie