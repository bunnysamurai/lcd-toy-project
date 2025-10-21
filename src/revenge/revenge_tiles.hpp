#if !defined(REVENGE_TILES_HPP)
#define REVENGE_TILES_HPP

#include "embp/constexpr_numeric.hpp"
#include "common/utilities.hpp"
#include "revenge_defs.hpp"
#include "screen/screen.hpp"

namespace revenge {

inline constexpr screen::Format VIDEO_FORMAT{screen::Format::RGB565_LUT4};

inline constexpr std::array Palette{
    /* clang-format off */
    screen::Clut{.r = 0, .g = 0, .b = 0},           /* black */
    screen::Clut{.r = 255, .g = 0, .b = 0},         /* red */
    screen::Clut{.r = 0, .g = 220, .b = 0},         /* green */
    screen::Clut{.r = 0, .g = 0, .b = 255},         /* blue */
    screen::Clut{.r = 0, .g = 220, .b = 220},       /* cyan */
    screen::Clut{.r = 220, .g = 220, .b = 0},       /* yellow */
    screen::Clut{.r = 245, .g = 0, .b = 245},       /* magenta */
    screen::Clut{.r = 84, .g = 84, .b = 84},        /* dark grey */

    screen::Clut{.r = 255, .g = 255, .b = 255},     /* white */
    screen::Clut{.r = 255, .g = 128, .b = 128},     /* light red */
    screen::Clut{.r = 200, .g = 255, .b = 200},     /* light green */
    screen::Clut{.r = 128, .g = 128, .b = 255},     /* light blue */
    screen::Clut{.r = 200, .g = 255, .b = 255},     /* light cyan */
    screen::Clut{.r = 255, .g = 255, .b = 200},     /* light yellow */
    screen::Clut{.r = 255, .g = 128, .b = 255},     /* light magenta */
    screen::Clut{.r = 188, .g = 188, .b = 188},     /* light grey */
    /* clang-format on */
};

inline constexpr uint8_t BLACK{0};
inline constexpr uint8_t RED{1};
inline constexpr uint8_t GREEN{2};
inline constexpr uint8_t BLUE{3};
inline constexpr uint8_t CYAN{4};
inline constexpr uint8_t YELLOW{5};
inline constexpr uint8_t MAGENTA{6};
inline constexpr uint8_t DRKGRY{7};
inline constexpr uint8_t WHITE{8};
inline constexpr uint8_t LRED{9};
inline constexpr uint8_t LGREEN{10};
inline constexpr uint8_t LBLUE{11};
inline constexpr uint8_t LCYAN{12};
inline constexpr uint8_t LYELLOW{13};
inline constexpr uint8_t LMAGENTA{14};
inline constexpr uint8_t LGREY{15};

/*
tiles required:
  NOTHING = 0b000,
  MOVABLE_BLOCK = 0b001,
  TRAP = 0b010,
  HOLE = 0b011,
  UNMOVEABLE_BLOCK = 0b100,
  CHEESE = 0b101
cat
mouse


     screen::Tile{.side_length = TetriminoTile_SideLength,
                             .format = VIDEO_FORMAT,
                             .data = std::data(Tetrimino_Tile_Data)},

*/

inline constexpr size_t BTLEN{(PIXELS_PER_GRID / 2) * PIXELS_PER_GRID};

/* tile data */
inline constexpr auto background_tile{
    embp::filled<uint8_t, BTLEN>(LYELLOW << 4 | LYELLOW)};
inline constexpr auto block_tile{
    embp::filled<uint8_t, BTLEN>(LGREEN << 4 | LGREEN)};
inline constexpr auto trap_tile{
    embp::filled<uint8_t, BTLEN>(LGREY << 4 | LGREY)};
inline constexpr auto hole_tile{
    embp::concat(
        embp::pfold(LYELLOW,LYELLOW, DRKGRY, DRKGRY, DRKGRY, DRKGRY, DRKGRY, DRKGRY,LYELLOW,LYELLOW ),
        embp::pfold(LYELLOW, DRKGRY, DRKGRY, DRKGRY, DRKGRY, DRKGRY, DRKGRY, DRKGRY, DRKGRY,LYELLOW ),
        embp::pfold( DRKGRY, DRKGRY, DRKGRY,  BLACK,  BLACK,  BLACK, DRKGRY, DRKGRY, DRKGRY, DRKGRY ),
        embp::pfold( DRKGRY, DRKGRY,  BLACK,  BLACK,  BLACK,  BLACK,  BLACK, DRKGRY, DRKGRY, DRKGRY ),
        embp::pfold( DRKGRY, DRKGRY,  BLACK,  BLACK,  BLACK,  BLACK,  BLACK, DRKGRY, DRKGRY, DRKGRY ),
        embp::pfold( DRKGRY, DRKGRY,  BLACK,  BLACK,  BLACK,  BLACK,  BLACK, DRKGRY, DRKGRY, DRKGRY ),
        embp::pfold( DRKGRY, DRKGRY,  BLACK,  BLACK,  BLACK,  BLACK,  BLACK, DRKGRY, DRKGRY, DRKGRY ),
        embp::pfold( DRKGRY, DRKGRY, DRKGRY,  BLACK,  BLACK,  BLACK, DRKGRY, DRKGRY, DRKGRY, DRKGRY ),
        embp::pfold(LYELLOW, DRKGRY, DRKGRY, DRKGRY, DRKGRY, DRKGRY, DRKGRY, DRKGRY, DRKGRY,LYELLOW ),
        embp::pfold(LYELLOW,LYELLOW, DRKGRY, DRKGRY, DRKGRY, DRKGRY, DRKGRY, DRKGRY,LYELLOW,LYELLOW )
    )
};
inline constexpr auto nonmoveblock_tile{
    embp::filled<uint8_t, BTLEN>(LCYAN << 4 | LCYAN)};
inline constexpr auto cheese_tile{
    embp::filled<uint8_t, BTLEN>(YELLOW << 4 | YELLOW)};
inline constexpr auto cat_tile{
    embp::concat(
        embp::filled<uint8_t, PIXELS_PER_GRID>(LYELLOW << 4 | LYELLOW),
        embp::pfold(LYELLOW,LYELLOW, YELLOW,LYELLOW,LYELLOW,LYELLOW, YELLOW,LYELLOW, YELLOW,LYELLOW ),
        embp::pfold(LYELLOW, YELLOW,LYELLOW,LYELLOW,LYELLOW,LYELLOW,LYELLOW, YELLOW, YELLOW,LYELLOW ),
        embp::pfold(LYELLOW,LYELLOW, YELLOW,LYELLOW,LYELLOW,LYELLOW,LYELLOW, YELLOW, YELLOW,LYELLOW ),
        embp::pfold(LYELLOW,LYELLOW,LYELLOW, YELLOW, YELLOW, YELLOW, YELLOW, YELLOW,LYELLOW,LYELLOW ),
        embp::pfold(LYELLOW,LYELLOW,LYELLOW, YELLOW, YELLOW, YELLOW, YELLOW, YELLOW,LYELLOW,LYELLOW ),
        embp::pfold(LYELLOW,LYELLOW,LYELLOW, YELLOW, YELLOW, YELLOW, YELLOW,LYELLOW,LYELLOW,LYELLOW ),
        embp::pfold(LYELLOW,LYELLOW, YELLOW, YELLOW,LYELLOW,LYELLOW, YELLOW, YELLOW,LYELLOW,LYELLOW ),
        embp::pfold(LYELLOW,LYELLOW, YELLOW, YELLOW,LYELLOW,LYELLOW, YELLOW, YELLOW,LYELLOW,LYELLOW ),
        embp::filled<uint8_t, PIXELS_PER_GRID>(LYELLOW << 4 | LYELLOW)
    )
};
inline constexpr auto sitting_cat_tile{embp::filled<uint8_t, BTLEN>(LRED << 4 | LRED)};

// clang-format off
inline constexpr auto mouse_tile{
    embp::concat(
        embp::filled<uint8_t, PIXELS_PER_GRID>(LYELLOW << 4 | LYELLOW),
        embp::pfold(LYELLOW,  LGREY,  LGREY,LYELLOW,  LGREY,  LGREY,LYELLOW,LYELLOW,LYELLOW,LYELLOW ),
        embp::pfold(LYELLOW,LYELLOW,  LGREY,  LGREY,LYELLOW,LYELLOW,LYELLOW,LYELLOW,LYELLOW,LYELLOW ),
        embp::pfold(LYELLOW,LYELLOW,  LGREY,  LGREY,  LGREY,LYELLOW,  LGREY,LYELLOW,  LGREY,LYELLOW ),
        embp::pfold(LYELLOW,  LGREY,LYELLOW,LYELLOW,  LGREY,  LGREY,  LGREY,  LGREY,LYELLOW,LYELLOW ),
        embp::pfold(LYELLOW,LYELLOW,LYELLOW,LYELLOW,  LGREY,  LGREY,  LGREY,LYELLOW,LYELLOW,LYELLOW ),
        embp::pfold(LYELLOW,LYELLOW,LYELLOW,  LGREY,LYELLOW,LYELLOW,LYELLOW,  LGREY,LYELLOW,LYELLOW ),
        embp::pfold(LYELLOW,LYELLOW,  LGREY,LYELLOW,LYELLOW,LYELLOW,LYELLOW,LYELLOW,  LGREY,LYELLOW ),
        embp::pfold(LYELLOW,LYELLOW,LYELLOW,LYELLOW,LYELLOW,LYELLOW,LYELLOW,LYELLOW,LYELLOW,LYELLOW ),
        embp::filled<uint8_t, PIXELS_PER_GRID>(LYELLOW << 4 | LYELLOW)
    )
};
inline constexpr auto mouse_stuck_tile{
    embp::concat(
        embp::pfold(LYELLOW,LYELLOW, DRKGRY, DRKGRY, DRKGRY, DRKGRY, DRKGRY, DRKGRY,LYELLOW,LYELLOW ),
        embp::pfold(LYELLOW,  LGREY,  LGREY, DRKGRY,  LGREY,  LGREY, DRKGRY, DRKGRY, DRKGRY,LYELLOW ),
        embp::pfold( DRKGRY, DRKGRY,  LGREY,  LGREY, DRKGRY, DRKGRY, DRKGRY, DRKGRY, DRKGRY, DRKGRY ),
        embp::pfold( DRKGRY, DRKGRY,  LGREY,  LGREY,  LGREY, DRKGRY,  LGREY, DRKGRY,  LGREY, DRKGRY ),
        embp::pfold( DRKGRY,  LGREY, DRKGRY, DRKGRY,  LGREY,  LGREY,  LGREY,  LGREY, DRKGRY, DRKGRY ),
        embp::pfold( DRKGRY, DRKGRY, DRKGRY, DRKGRY,  LGREY,  LGREY,  LGREY, DRKGRY, DRKGRY, DRKGRY ),
        embp::pfold( DRKGRY, DRKGRY, DRKGRY,  LGREY, DRKGRY, DRKGRY, DRKGRY,  LGREY, DRKGRY, DRKGRY ),
        embp::pfold( DRKGRY, DRKGRY,  LGREY, DRKGRY, DRKGRY, DRKGRY, DRKGRY, DRKGRY,  LGREY, DRKGRY ),
        embp::pfold(LYELLOW, DRKGRY, DRKGRY, DRKGRY, DRKGRY, DRKGRY, DRKGRY, DRKGRY, DRKGRY,LYELLOW ),
        embp::pfold(LYELLOW,LYELLOW, DRKGRY, DRKGRY, DRKGRY, DRKGRY, DRKGRY, DRKGRY,LYELLOW,LYELLOW )
    )
};
// clang-format on


/* screen::Tile objects */
inline constexpr screen::Tile BACKGROUND{.side_length = PIXELS_PER_GRID,
                                         .format = VIDEO_FORMAT,
                                         .data = std::data(background_tile)};
inline constexpr screen::Tile BLOCK{.side_length = PIXELS_PER_GRID,
                                    .format = VIDEO_FORMAT,
                                    .data = std::data(block_tile)};
inline constexpr screen::Tile TRAP{.side_length = PIXELS_PER_GRID,
                                   .format = VIDEO_FORMAT,
                                   .data = std::data(trap_tile)};
inline constexpr screen::Tile HOLE{.side_length = PIXELS_PER_GRID,
                                   .format = VIDEO_FORMAT,
                                   .data = std::data(hole_tile)};
inline constexpr screen::Tile UNMOVEBLOCK{.side_length = PIXELS_PER_GRID,
                                          .format = VIDEO_FORMAT,
                                          .data = std::data(nonmoveblock_tile)};
inline constexpr screen::Tile CHEESE{.side_length = PIXELS_PER_GRID,
                                     .format = VIDEO_FORMAT,
                                     .data = std::data(cheese_tile)};
inline constexpr screen::Tile CAT{.side_length = PIXELS_PER_GRID,
                                  .format = VIDEO_FORMAT,
                                  .data = std::data(cat_tile)};
inline constexpr screen::Tile SITTING_CAT{.side_length = PIXELS_PER_GRID,
                                  .format = VIDEO_FORMAT,
                                  .data = std::data(sitting_cat_tile)};
inline constexpr screen::Tile MOUSE{.side_length = PIXELS_PER_GRID,
                                    .format = VIDEO_FORMAT,
                                    .data = std::data(mouse_tile)};
inline constexpr screen::Tile MOUSE_IN_HOLE{.side_length = PIXELS_PER_GRID,
                                    .format = VIDEO_FORMAT,
                                    .data = std::data(mouse_stuck_tile)};
} // namespace revenge

#endif
