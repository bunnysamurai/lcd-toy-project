#ifndef LEVEL_UTILS_HPP
#define LEVEL_UTILS_HPP

#include "chippie/state/state.hpp"

#include <cstdint>

namespace chippie
{

constexpr void draw_h_line(state &game_state, terrain_type terrain, uint32_t row, uint32_t col,
                           uint32_t length) noexcept
{
    for (uint32_t xx = col; xx < col + length; ++xx)
    {
        game_state.the_map[Grid::Location{.x = xx, .y = row}] = terrain;
        game_state.the_map[Grid::Location{.x = xx, .y = row}] = terrain;
    }
}

constexpr void draw_v_line(state &game_state, terrain_type terrain, uint32_t row, uint32_t col,
                           uint32_t length) noexcept
{
    for (uint32_t yy = row; yy < row + length; ++yy)
    {
        game_state.the_map[Grid::Location{.x = col, .y = yy}] = terrain;
        game_state.the_map[Grid::Location{.x = col, .y = yy}] = terrain;
    }
}

} // namespace chippie
#endif