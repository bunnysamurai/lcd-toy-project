#if !defined(SNAKE_TILES_CONSTEXPR_HPP)
#define SNAKE_TILES_CONSTEXPR_HPP

#include <cstddef>

#include "screen/TileDef.h"
#include "screen/constexpr_tile_utils.hpp"

namespace snake {
inline constexpr size_t BorderTile_SideLength{7U};
inline constexpr auto Border_Tile_Data{
    constexpr_screen::fill_with_rgb565<BorderTile_SideLength *
                                       BorderTile_SideLength * 2>(
        constexpr_screen::Color::BLUE)};

inline constexpr screen::Tile BorderTile{.side_length = BorderTile_SideLength,
                                         .format = screen::Format::RGB565,
                                         .data = std::data(Border_Tile_Data)};

inline constexpr size_t SnakeTile_SideLength{BorderTile_SideLength};
/* clang-format off */
inline constexpr std::array<uint8_t, SnakeTile_SideLength *
                                    SnakeTile_SideLength * 2> Snake_Tile_Data{
    0x00,0x00, 0xE0,0x07, 0xE0,0x07, 0xE0,0x07, 0xE0,0x07, 0xE0,0x07, 0x00,0x00,
    0x00,0x00, 0xE0,0x07, 0xE0,0x07, 0xE0,0x07, 0xE0,0x07, 0xE0,0x07, 0x00,0x00,
    0x00,0x00, 0xE0,0x07, 0xE0,0x07, 0xE0,0x07, 0xE0,0x07, 0xE0,0x07, 0x00,0x00,
    0x00,0x00, 0xE0,0x07, 0xE0,0x07, 0xE0,0x07, 0xE0,0x07, 0xE0,0x07, 0x00,0x00,
    0x00,0x00, 0xE0,0x07, 0xE0,0x07, 0xE0,0x07, 0xE0,0x07, 0xE0,0x07, 0x00,0x00,
    0x00,0x00, 0xE0,0x07, 0xE0,0x07, 0xE0,0x07, 0xE0,0x07, 0xE0,0x07, 0x00,0x00,
    0x00,0x00, 0xE0,0x07, 0xE0,0x07, 0xE0,0x07, 0xE0,0x07, 0xE0,0x07, 0x00,0x00,
};
/* clang-format on */

inline constexpr screen::Tile SnakeTile{.side_length = SnakeTile_SideLength,
                                        .format = screen::Format::RGB565,
                                        .data = std::data(Snake_Tile_Data)};

inline constexpr size_t AppleTile_SideLength{BorderTile_SideLength};
inline constexpr auto Apple_Tile_Data{
    constexpr_screen::fill_with_rgb565<AppleTile_SideLength *
                                       AppleTile_SideLength * 2>(
        constexpr_screen::Color::RED)};

inline constexpr screen::Tile AppleTile{.side_length = AppleTile_SideLength,
                                        .format = screen::Format::RGB565,
                                        .data = std::data(Apple_Tile_Data)};

inline constexpr size_t BackgroundTile_SideLength{BorderTile_SideLength};
inline constexpr auto Background_Tile_Data{
    constexpr_screen::fill_with_rgb565<BackgroundTile_SideLength *
                                       BackgroundTile_SideLength * 2>(
        constexpr_screen::Color::BLACK)};

inline constexpr screen::Tile BackgroundTile{
    .side_length = BackgroundTile_SideLength,
    .format = screen::Format::RGB565,
    .data = std::data(Background_Tile_Data)};
} // namespace snake

#endif