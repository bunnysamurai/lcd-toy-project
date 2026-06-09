#if !defined(STATE_HPP)
#define STATE_HPP

#include "chippie/chippie_common.hpp"
#include "chippie/entities/basic_entity.hpp"
#include "common/pico_sdk_timer_details.hpp"
#include "common/timer.hpp"
#include "embp/containers.hpp"
#include "screen/gfx/defs.hpp"
#include "static_map.hpp"

#include "pico/time.h"

#include <cstdint>

namespace chippie
{

/* this guy has gotten a little bloated.  need to break some of this up */
struct State
{
  public:
    State() noexcept;

    [[nodiscard]] bool is_active() noexcept;

    void load_level() noexcept;

    void process() noexcept;

    void decrement_chip_count() noexcept;

    [[nodiscard]] uint16_t get_chip_count() noexcept;

  private:
    void load_entity_list_from_rom_stub([[maybe_unused]] uint8_t level) noexcept;

    void load_level_from_rom_stub(uint8_t level) noexcept;

    void process_opening_level_dialog() noexcept;

    void move_entities() noexcept;

    [[nodiscard]] collision_result find_collisions(Grid::Location next_location) noexcept;

    void update_view_port_position() noexcept;

    [[nodiscard]] bool within_view_port(Grid::Location loc) noexcept;

    void process_time_remaining() noexcept;

    void paint() noexcept;

    void paint_the_background() noexcept;
    void paint_the_map() noexcept;
    void paint_chip_count_display() noexcept;
    void paint_timer_display() noexcept;
    void paint_level_display() noexcept;
    void paint_hint() noexcept;
    void paint_inventory() noexcept;

    void paint_entity(const entity &ent) const noexcept;

    [[nodiscard]] Grid::Location make_relative_to_view_port(Grid::Location abs_loc) const noexcept;

    void init_play_grid() noexcept;

    void intialize_event_handlers() noexcept;

    /* list of event handler functions */
    void handle_portal() noexcept;
    void handle_time_up() noexcept;
    void handle_display_hint() noexcept;
    void handle_clear_hint() noexcept;

    bool active{true}; /* set to false when game should end */
    uint8_t level_number{1};
    screen::gfx::Rect_<uint32_t> view_port{
        .topleft = {.x = 0, .y = 0},
        .size = {.width = 9, .height = 9},
    }; /* rectangle of the viewable portion of the grid */
    Grid play_grid;
    Grid view_grid;
    Grid chip_count_grid;
    Grid level_count_grid;
    Grid time_remaining_grid;
    Grid hint_print_grid;
    Grid inventory_grid;
    Timer<timer_details::PicoSdk> countdown_timer{1'000'000}; /* period of 1 second*/
    absolute_time_t last_paint_time{};
    static constexpr int64_t PAINT_TIME_INTERVAL_US{33'333};
    bool hint_displayable{false};
    screen::gfx::Rect hint_area;
    bool display_level_name_once{true};

    /* level state */
    const char *hint_text;
    const char *level_name_text;
    uint16_t time_remaining{};
    static_map the_map;
    embp::variable_array<entity, 128> entity_list;
};

} // namespace chippie

#endif