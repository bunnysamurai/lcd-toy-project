#if !defined(SNAKE_TILES_CONSTEXPR_HPP)
#define SNAKE_TILES_CONSTEXPR_HPP

#include <cstddef>

#include "screen/TileDef.h"
#include "screen/constexpr_tile_utils.hpp"

namespace snake {
inline constexpr size_t BorderTile_SideLength{4U};
inline constexpr auto Border_Tile_Data{
    constexpr_screen::fill_with_rgb565<BorderTile_SideLength *
                                       BorderTile_SideLength * 2>(
        constexpr_screen::Color::BLUE)};

inline constexpr screen::Tile BorderTile{.side_length = BorderTile_SideLength,
                                         .format = screen::Format::RGB565,
                                         .data = std::data(Border_Tile_Data)};

} // namespace snake

#endif