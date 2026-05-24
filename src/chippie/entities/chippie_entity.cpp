#include "chippie_entity.hpp"
#include "chippie/chippie_common.hpp"
#include "chippie/events/event.hpp"
#include "gamepad/gamepad.hpp"

#include "pico/time.h"
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

[[nodiscard]] collision_action handle_collision(entity &ent, entity *collided_entity,
                                                terrain_type &collided_terrain) noexcept
{
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
entity create(Grid::Location xy, direction dir, uint8_t uuid) noexcept
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
