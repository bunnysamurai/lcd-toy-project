#if !defined(PURPLE_BALL_HPP)
#define PURPLE_BALL_HPP

#include "basic_entity.hpp"
#include "chippie/chippie_common.hpp"
#include "chippie/collision.hpp"
#include "chippie/events/event.hpp"

namespace chippie
{

struct purple_ball final : public entity
{
  public:
    purple_ball(Grid::Location location, direction init_facing)
        : facing{init_facing}, entity{location, entity_type::PURPLE_BALL}
    {
    }

  private:
    [[nodiscard]] Grid::Location compute_next_location() const noexcept override
    {
        auto nextloc{loc};

        switch (facing)
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

    void handle_collision(collision_status collision, Grid::Location nextloc) noexcept override
    {
        /* in the case of the purple ball:
                if it's clear, allow the move,
                if it's the player, game over,
                otherwise, reverse facing and move again
        */
        for (uint32_t tries = 0; tries < 2; ++tries)
        {
            switch (collision.type)
            {
            case entity_type::CHIPPIE:
                register_event(event::event_type::GAME_OVER);
                break;
            case entity_type::WALL:
                facing = reverse(facing);
                nextloc = compute_next_location();
                collision = check_for_collision(this, nextloc);
                break;
            default:
                loc = nextloc;
                break;
            }
        }
    }

  private:
    direction facing;
};

} // namespace chippie

#endif