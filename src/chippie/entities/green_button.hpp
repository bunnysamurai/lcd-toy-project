#if !defined(GREEN_BUTTON_HPP)
#define GREEN_BUTTON_HPP

#include "basic_entity.hpp"
#include "chippie/events/event.hpp"

namespace chippie
{
struct green_button final : public entity
{
  public:
    green_button(Grid::Location location) noexcept : entity(location, entity_type::GREEN_BUTTON)
    {
    }

  private:
    Grid::Location compute_next_location() const noexcept override
    {
        /* doesn't move */
        return loc;
    }

    void handle_collision(collision_status collision, Grid::Location nextloc) noexcept override
    {
        /* for the case of green button, doesn't matter what collided with us,
            we invert the state of our wall locations.
        */

        if (!button_pressed && collision.type != entity_type::CLEAR)
        {
            event::register_event(event::event_type::GREEN_BUTTON);
            button_pressed = true;
            return;
        }

        if (button_pressed && collision.type == entity_type::CLEAR)
        {
            button_pressed = false;
        }
    }

  private:
    bool button_pressed{false};
};

} // namespace chippie
#endif