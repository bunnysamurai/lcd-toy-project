#if !defined(STATE_HPP)
#define STATE_HPP

#include "chippie/chippie_common.hpp"
#include "chippie/entities/basic_entity.hpp"
#include "chippie_inventory.hpp"

#include "common/time_utils.hpp"

#include "embp/containers.hpp"

#include "screen/gfx/defs.hpp"

#include "red_brown_buttons.hpp"
#include "static_map.hpp"
#include "teleporter.hpp"

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
    /* used by the "Time Remaining" element on the game's GUI */
    game_clock_t the_clock;
    embp::timer<game_clock_t&> countdown_timer{COUNTDOWN_US, the_clock};
    steady_clock_source::time_base_t last_paint_time{};
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
    embp::variable_array<red_button, 7> red_button_list;
    embp::variable_array<brown_button, 18> brown_button_list;
    embp::variable_array<teleporter, 9> teleport_list;
};

void paint(state &) noexcept;

} // namespace chippie

#endif