#if !defined(STATE_HPP)
#define STATE_HPP

#include "chippie/chippie_common.hpp"
#include "chippie/entities/basic_entity.hpp"
#include "chippie_inventory.hpp"

#include "common/pico_sdk_timer_details.hpp"
#include "common/timer.hpp"

#include "embp/containers.hpp"

#include "screen/gfx/defs.hpp"

#include "red_brown_buttons.hpp"
#include "static_map.hpp"
#include "teleporter.hpp"

#include "pico/time.h"

#include <cstdint>

namespace chippie
{

struct state
{
    enum struct reason
    {
        CHIP_DIED,
        PORTAL_REACHED,
        RESTART_LEVEL,
        USER_QUIT
    };
    bool active{true}; /* set to false when game should end */
    uint8_t level_number{1};
    screen::gfx::Rect_<uint32_t> view_port{
        .topleft = {.x = 0, .y = 0},
        .size = {.width = 9, .height = 9},
    }; /* rectangle of the viewable portion of the play_grid */

    /* these are basically controlling the layout of the game's visual elements */
    Grid play_grid; /* defines the full 32x32 map */
    Grid view_grid; /* defines where on the display to draw the map */
    Grid chip_count_grid;
    Grid level_count_grid;
    Grid time_remaining_grid;
    Grid hint_print_grid;
    Grid inventory_grid;

    static constexpr int COUNTDOWN_US{1'000'000};
    Timer<timer_details::PicoSdk> countdown_timer{COUNTDOWN_US}; /* period of 1 second*/
    absolute_time_t last_paint_time{};
    bool hint_displayable{false};
    screen::gfx::Rect hint_area;
    bool display_level_name_once{true};
    inventory chippie_inventory;
    reason inactive_reason;

    /* level state */
    const char *hint_text;
    const char *level_name_text;
    int16_t time_remaining{};
    static_map the_map;
    embp::variable_array<entity, 128> entity_list;
    embp::variable_array<red_button, 32> red_button_list;
    embp::variable_array<brown_button, 32> brown_button_list;
    embp::variable_array<teleporter, 4> teleport_list;
};

void paint(state &) noexcept;

} // namespace chippie

#endif