#if !defined(STATE_HPP)
#define STATE_HPP

#include "chippie/chippie_common.hpp"
#include "chippie/entities/basic_entity.hpp"
#include "common/Rect.hpp"
#include "embp/containers.hpp"
#include "static_map.hpp"

#include "pico/time.h"

#include <cstdint>

namespace chippie
{

struct State
{
  public:
    [[nodiscard]] bool is_active() noexcept;

    void load_level() noexcept;

    void process() noexcept;

    void decrement_chip_count() noexcept;

    [[nodiscard]] uint16_t get_chip_count() noexcept;

  private:
    void load_entity_list_from_rom_stub([[maybe_unused]] uint8_t level) noexcept;

    void load_level_from_rom_stub(uint8_t level) noexcept;

    void move_entities() noexcept;

    [[nodiscard]] collision_result find_collisions(Grid::Location next_location) noexcept;

    void update_view_port_position() noexcept;

    [[nodiscard]] bool within_view_port(Grid::Location loc) noexcept;

    void paint() noexcept;

    void paint_the_map() noexcept;
    void paint_chip_count_display() noexcept;
    void paint_timer_display() noexcept;
    void paint_level_display() noexcept;

    void paint_entity(const entity &ent) const noexcept;

    [[nodiscard]] Grid::Location make_relative_to_view_port(Grid::Location abs_loc) const noexcept;

    void init_play_grid() noexcept;

    bool active{true}; /* set to false when game should end */
    uint8_t level_number{1};
    Rect_<uint32_t> view_port{
        .x = 0, .y = 0, .width = 9, .height = 9}; /* rectangle of the viewable portion of the grid */
    embp::variable_array<entity, 128> entity_list;
    Grid play_grid;
    Grid view_grid;
    uint16_t chip_count;
    static_map the_map;
    absolute_time_t last_paint_time{};
    static constexpr int64_t PAINT_TIME_INTERVAL_US{33'333};
};

} // namespace chippie

#endif