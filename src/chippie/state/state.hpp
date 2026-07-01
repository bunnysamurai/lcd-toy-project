#if !defined(STATE_HPP)
#define STATE_HPP

#include "chippie/chippie_common.hpp"
#include "chippie/entities/basic_entity.hpp"
#include "chippie_inventory.hpp"

#include "common/pico_sdk_timer_details.hpp"
#include "common/timer.hpp"

#include "embp/containers.hpp"

#include "screen/gfx/defs.hpp"

#include "static_map.hpp"

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
        USER_QUIT
    };
    bool active{true}; /* set to false when game should end */
    uint8_t level_number{1};
    screen::gfx::Rect_<uint32_t> view_port{
        .topleft = {.x = 0, .y = 0},
        .size = {.width = 9, .height = 9},
    }; /* rectangle of the viewable portion of the grid */

    /* these are basically controlling the layout of the game's visual elements */
    Grid play_grid;
    Grid view_grid;
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
    uint16_t time_remaining{};
    static_map the_map;
    embp::variable_array<entity, 128> entity_list;
    /* how to handle the buttons? 
        For green, we can either 
            * iterate through the entire map, inverting state as we go along
            * store a list of just those locations controlled by the green button, and iterate through that
            
            I'll go with the simpler but less efficient approach (iterate through the entire map) as I don't think it
            will impact performance significantly... I'll measure, of course, and report back here.
    */
};

void paint(state &) noexcept;

} // namespace chippie

#endif