#if !defined(SNAKE_TILES_CONSTEXPR_HPP)
#define SNAKE_TILES_CONSTEXPR_HPP

#include <array>
#include <cstddef>
#include <cstdint>

#include "screen/TileDef.h"
#include "screen/constexpr_tile_utils.hpp"
#include "screen/screen_def.h"

namespace snake {

inline constexpr size_t GRID_SPACE_PIX{7U};

/* snake's color palette */
inline constexpr std::array Palette{
    /* clang-format off */
    screen::Clut{.r = 0, .g = 0, .b = 0},           /* black */
    screen::Clut{.r = 0xCF, .g = 0xCF, .b = 0},     /* snake skin */
    screen::Clut{.r = 250, .g = 250, .b = 250},     /* snake tail, bright */
    screen::Clut{.r = 250, .g = 250, .b = 250},     /* snake tail, dim */
    screen::Clut{.r = 0, .g = 0, .b = 255},         /* blue border, snake eye */
    screen::Clut{.r = 255, .g = 0, .b = 0},         /* red */
    screen::Clut{.r = 200, .g = 0, .b = 0},         /* dim red */
    screen::Clut{.r = 255, .g = 255, .b = 0},       /* yel, snake shine */
    screen::Clut{.r = 0, .g = 0, .b = 200},         /* blue border shade */
    screen::Clut{.r = 255>>1, .g = 255>>1, .b = 0}, /* snake shadow */
    screen::Clut{.r = 0, .g = 255, .b = 0}, /* timer green */
    /* clang-format on */
};
inline constexpr uint8_t BLACK{0};
inline constexpr uint8_t SKIN{1};
inline constexpr uint8_t GREEN{SKIN};
inline constexpr uint8_t TAIL_BRIGHT{2};
inline constexpr uint8_t TAIL_DIM{3};
inline constexpr uint8_t EYE{4};
inline constexpr uint8_t BORDER{EYE};
inline constexpr uint8_t RED{5};
inline constexpr uint8_t DIMRED{6};
inline constexpr uint8_t YEL{7};
inline constexpr uint8_t SKINSHN{YEL};
inline constexpr uint8_t BDRDIM{8};
inline constexpr uint8_t SDWSKIN{9};
inline constexpr uint8_t TIMEGRN{10};

inline constexpr screen::Format TILE_FORMAT{screen::Format::RGB565_LUT4};

/* =====================================================================
                 ____                _
                | __ )  ___  _ __ __| | ___ _ __
                |  _ \ / _ \| '__/ _` |/ _ \ '__|
                | |_) | (_) | | | (_| |  __/ |
                |____/ \___/|_|  \__,_|\___|_|

 * ===================================================================== */
/*
        4-way mapping

              t
        l     c     r
              b
    t, r, b, l
    tr, tb, tl
    rb, rl
    bl
    trb, lrb, blt, ltr
    trbl
    none, i.e. just c

    can be represented by a 4 bit number
       trbl
    BT_0000  :  just c
    BT_0001  :  l
    BT_0010  :  b
    BT_0011  :  bl
    BT_0100  :  r
    BT_0101  :  rl
    BT_0110  :  rb
    BT_0111  :  rbl
    BT_1000  :  t
    BT_1001  :  tl
    BT_1010  :  tb
    BT_1011  :  tbl
    BT_1100  :  tr
    BT_1101  :  trl
    BT_1110  :  trb
    BT_1111  :  trbl

 */
inline constexpr size_t BorderTile_SideLength{GRID_SPACE_PIX};
inline constexpr size_t BTLEN{
    (BorderTile_SideLength * (BorderTile_SideLength + 1)) / 2};
inline constexpr std::array Border_Tile_Data{
    /* clang-format off */
embp::concat(
    /* BT_0000 : c  */
    embp::pfold(BORDER,BORDER,BORDER,BORDER,BORDER,BORDER,BDRDIM,0,
                BORDER,BORDER,BORDER,BORDER,BORDER,BDRDIM,BDRDIM,0,
                BORDER,BORDER,BDRDIM,BORDER,BDRDIM,BDRDIM,BDRDIM,0,
                BORDER,BORDER,BORDER,BDRDIM,BORDER,BDRDIM,BDRDIM,0,
                BORDER,BORDER,BDRDIM,BORDER,BDRDIM,BDRDIM,BDRDIM,0,
                BORDER,BDRDIM,BDRDIM,BDRDIM,BDRDIM,BDRDIM,BDRDIM,0, 
                BDRDIM,BDRDIM,BDRDIM,BDRDIM,BDRDIM,BDRDIM,BDRDIM,0),
    /* BT_0001 : l  */
    embp::pfold(BORDER,BORDER,BORDER,BORDER,BORDER,BORDER,BDRDIM,0,
                BORDER,BORDER,BORDER,BORDER,BORDER,BDRDIM,BDRDIM,0,
                BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,BDRDIM,BDRDIM,0,
                BORDER,BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,BDRDIM,0,
                BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,BDRDIM,BDRDIM,0,
                BDRDIM,BDRDIM,BDRDIM,BDRDIM,BDRDIM,BDRDIM,BDRDIM,0, 
                BDRDIM,BDRDIM,BDRDIM,BDRDIM,BDRDIM,BDRDIM,BDRDIM,0),
    /* BT_0010 : b  */
    embp::pfold(BORDER,BORDER,BORDER,BORDER,BORDER,BORDER,BDRDIM,0,
                BORDER,BORDER,BORDER,BORDER,BORDER,BDRDIM,BDRDIM,0,
                BORDER,BORDER,BDRDIM,BORDER,BDRDIM,BDRDIM,BDRDIM,0,
                BORDER,BORDER,BORDER,BDRDIM,BORDER,BDRDIM,BDRDIM,0,
                BORDER,BORDER,BDRDIM,BORDER,BDRDIM,BDRDIM,BDRDIM,0,
                BORDER,BORDER,BORDER,BDRDIM,BORDER,BDRDIM,BDRDIM,0, 
                BORDER,BORDER,BDRDIM,BORDER,BDRDIM,BDRDIM,BDRDIM,0),
    /* BT_0011 : bl */
    embp::pfold(BORDER,BORDER,BORDER,BORDER,BORDER,BORDER,BDRDIM,0,
                BORDER,BORDER,BORDER,BORDER,BORDER,BDRDIM,BDRDIM,0,
                BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,BDRDIM,BDRDIM,0,
                BORDER,BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,BDRDIM,0,
                BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,BDRDIM,BDRDIM,0,
                BORDER,BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,BDRDIM,0, 
                BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,BDRDIM,BDRDIM,0),
    /* BT_0100 : r  */
    embp::pfold(BORDER,BORDER,BORDER,BORDER,BORDER,BORDER,BORDER,0,
                BORDER,BORDER,BORDER,BORDER,BORDER,BORDER,BORDER,0,
                BORDER,BORDER,BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,0,
                BORDER,BORDER,BORDER,BDRDIM,BORDER,BDRDIM,BORDER,0,
                BORDER,BORDER,BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,0,
                BORDER,BDRDIM,BDRDIM,BDRDIM,BDRDIM,BDRDIM,BDRDIM,0, 
                BDRDIM,BDRDIM,BDRDIM,BDRDIM,BDRDIM,BDRDIM,BDRDIM,0),
    /* BT_0101 : rl */
    embp::pfold(BORDER,BORDER,BORDER,BORDER,BORDER,BORDER,BORDER,0,
                BORDER,BORDER,BORDER,BORDER,BORDER,BORDER,BORDER,0,
                BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,0,
                BORDER,BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,BORDER,0,
                BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,0,
                BDRDIM,BDRDIM,BDRDIM,BDRDIM,BDRDIM,BDRDIM,BDRDIM,0, 
                BDRDIM,BDRDIM,BDRDIM,BDRDIM,BDRDIM,BDRDIM,BDRDIM,0),
    /* BT_0110 : rb */
    embp::pfold(BORDER,BORDER,BORDER,BORDER,BORDER,BORDER,BORDER,0,
                BORDER,BORDER,BORDER,BORDER,BORDER,BORDER,BORDER,0,
                BORDER,BORDER,BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,0,
                BORDER,BORDER,BORDER,BDRDIM,BORDER,BDRDIM,BORDER,0,
                BORDER,BORDER,BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,0,
                BORDER,BORDER,BORDER,BDRDIM,BORDER,BDRDIM,BORDER,0, 
                BORDER,BORDER,BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,0),
    /* BT_0111 : rbl*/
    embp::pfold(BORDER,BORDER,BORDER,BORDER,BORDER,BORDER,BORDER,0,
                BORDER,BORDER,BORDER,BORDER,BORDER,BORDER,BORDER,0,
                BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,0,
                BORDER,BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,BORDER,0,
                BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,0,
                BORDER,BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,BORDER,0, 
                BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,0),
    /* BT_1000 : t  */
    embp::pfold(BORDER,BORDER,BDRDIM,BORDER,BDRDIM,BDRDIM,BDRDIM,0,
                BORDER,BORDER,BORDER,BDRDIM,BORDER,BDRDIM,BDRDIM,0,
                BORDER,BORDER,BDRDIM,BORDER,BDRDIM,BDRDIM,BDRDIM,0,
                BORDER,BORDER,BORDER,BDRDIM,BORDER,BDRDIM,BDRDIM,0,
                BORDER,BORDER,BDRDIM,BORDER,BDRDIM,BDRDIM,BDRDIM,0,
                BORDER,BDRDIM,BDRDIM,BDRDIM,BDRDIM,BDRDIM,BDRDIM,0, 
                BDRDIM,BDRDIM,BDRDIM,BDRDIM,BDRDIM,BDRDIM,BDRDIM,0),
    /* BT_1001 : tl */
    embp::pfold(BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,BDRDIM,BDRDIM,0,
                BORDER,BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,BDRDIM,0,
                BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,BDRDIM,BDRDIM,0,
                BORDER,BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,BDRDIM,0,
                BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,BDRDIM,BDRDIM,0,
                BDRDIM,BDRDIM,BDRDIM,BDRDIM,BDRDIM,BDRDIM,BDRDIM,0, 
                BDRDIM,BDRDIM,BDRDIM,BDRDIM,BDRDIM,BDRDIM,BDRDIM,0),
    /* BT_1010 : tb */
    embp::pfold(BORDER,BORDER,BDRDIM,BORDER,BDRDIM,BDRDIM,BDRDIM,0,
                BORDER,BORDER,BORDER,BDRDIM,BORDER,BDRDIM,BDRDIM,0,
                BORDER,BORDER,BDRDIM,BORDER,BDRDIM,BDRDIM,BDRDIM,0,
                BORDER,BORDER,BORDER,BDRDIM,BORDER,BDRDIM,BDRDIM,0,
                BORDER,BORDER,BDRDIM,BORDER,BDRDIM,BDRDIM,BDRDIM,0,
                BORDER,BORDER,BORDER,BDRDIM,BORDER,BDRDIM,BDRDIM,0, 
                BORDER,BORDER,BDRDIM,BORDER,BDRDIM,BDRDIM,BDRDIM,0),
    /* BT_1011 : tbl */
    embp::pfold(BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,BDRDIM,BDRDIM,0,
                BORDER,BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,BDRDIM,0,
                BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,BDRDIM,BDRDIM,0,
                BORDER,BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,BDRDIM,0,
                BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,BDRDIM,BDRDIM,0,
                BORDER,BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,BDRDIM,0, 
                BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,BDRDIM,BDRDIM,0),
    /* BT_1100 : tr  */
    embp::pfold(BORDER,BORDER,BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,0,
                BORDER,BORDER,BORDER,BDRDIM,BORDER,BDRDIM,BORDER,0,
                BORDER,BORDER,BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,0,
                BORDER,BORDER,BORDER,BDRDIM,BORDER,BDRDIM,BORDER,0,
                BORDER,BORDER,BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,0,
                BORDER,BDRDIM,BDRDIM,BDRDIM,BDRDIM,BDRDIM,BDRDIM,0, 
                BDRDIM,BDRDIM,BDRDIM,BDRDIM,BDRDIM,BDRDIM,BDRDIM,0),
    /* BT_1101 : trl */
    embp::pfold(BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,0,
                BORDER,BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,BORDER,0,
                BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,0,
                BORDER,BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,BORDER,0,
                BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,0,
                BORDER,BDRDIM,BDRDIM,BDRDIM,BDRDIM,BDRDIM,BDRDIM,0, 
                BDRDIM,BDRDIM,BDRDIM,BDRDIM,BDRDIM,BDRDIM,BDRDIM,0),
    /* BT_1110 : trb */
    embp::pfold(BORDER,BORDER,BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,0,
                BORDER,BORDER,BORDER,BDRDIM,BORDER,BDRDIM,BORDER,0,
                BORDER,BORDER,BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,0,
                BORDER,BORDER,BORDER,BDRDIM,BORDER,BDRDIM,BORDER,0,
                BORDER,BORDER,BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,0,
                BORDER,BORDER,BORDER,BDRDIM,BORDER,BDRDIM,BORDER,0, 
                BORDER,BORDER,BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,0),
    /* BT_1111 : trbl*/
    embp::pfold(BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,0,
                BORDER,BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,BORDER,0,
                BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,0,
                BORDER,BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,BORDER,0,
                BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,0,
                BORDER,BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,BORDER,0, 
                BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,BORDER,BDRDIM,0)
)
    /* clang-format on */
};
static_assert(std::size(Border_Tile_Data) == BTLEN * 16);

inline constexpr std::array BorderTiles{
    screen::Tile{.side_length = BorderTile_SideLength,
                 .format = TILE_FORMAT,
                 .data = std::data(Border_Tile_Data)},
    screen::Tile{.side_length = BorderTile_SideLength,
                 .format = TILE_FORMAT,
                 .data = std::next(std::data(Border_Tile_Data), BTLEN)},
    screen::Tile{.side_length = BorderTile_SideLength,
                 .format = TILE_FORMAT,
                 .data = std::next(std::data(Border_Tile_Data), 2 * BTLEN)},
    screen::Tile{.side_length = BorderTile_SideLength,
                 .format = TILE_FORMAT,
                 .data = std::next(std::data(Border_Tile_Data), 3 * BTLEN)},
    screen::Tile{.side_length = BorderTile_SideLength,
                 .format = TILE_FORMAT,
                 .data = std::next(std::data(Border_Tile_Data), 4 * BTLEN)},
    screen::Tile{.side_length = BorderTile_SideLength,
                 .format = TILE_FORMAT,
                 .data = std::next(std::data(Border_Tile_Data), 5 * BTLEN)},
    screen::Tile{.side_length = BorderTile_SideLength,
                 .format = TILE_FORMAT,
                 .data = std::next(std::data(Border_Tile_Data), 6 * BTLEN)},
    screen::Tile{.side_length = BorderTile_SideLength,
                 .format = TILE_FORMAT,
                 .data = std::next(std::data(Border_Tile_Data), 7 * BTLEN)},
    screen::Tile{.side_length = BorderTile_SideLength,
                 .format = TILE_FORMAT,
                 .data = std::next(std::data(Border_Tile_Data), 8 * BTLEN)},
    screen::Tile{.side_length = BorderTile_SideLength,
                 .format = TILE_FORMAT,
                 .data = std::next(std::data(Border_Tile_Data), 9 * BTLEN)},
    screen::Tile{.side_length = BorderTile_SideLength,
                 .format = TILE_FORMAT,
                 .data = std::next(std::data(Border_Tile_Data), 10 * BTLEN)},
    screen::Tile{.side_length = BorderTile_SideLength,
                 .format = TILE_FORMAT,
                 .data = std::next(std::data(Border_Tile_Data), 11 * BTLEN)},
    screen::Tile{.side_length = BorderTile_SideLength,
                 .format = TILE_FORMAT,
                 .data = std::next(std::data(Border_Tile_Data), 12 * BTLEN)},
    screen::Tile{.side_length = BorderTile_SideLength,
                 .format = TILE_FORMAT,
                 .data = std::next(std::data(Border_Tile_Data), 13 * BTLEN)},
    screen::Tile{.side_length = BorderTile_SideLength,
                 .format = TILE_FORMAT,
                 .data = std::next(std::data(Border_Tile_Data), 14 * BTLEN)},
    screen::Tile{.side_length = BorderTile_SideLength,
                 .format = TILE_FORMAT,
                 .data = std::next(std::data(Border_Tile_Data), 15 * BTLEN)},
};
static_assert(std::size(BorderTiles) == 16);

/* =====================================================================
                     _   _                _
                    | | | | ___  __ _  __| |
                    | |_| |/ _ \/ _` |/ _` |
                    |  _  |  __/ (_| | (_| |
                    |_| |_|\___|\__,_|\__,_|

 * ===================================================================== */

inline constexpr size_t SnakeTile_SideLength{GRID_SPACE_PIX};
inline constexpr size_t SNAKETILE_DATALENGTH{
    (SnakeTile_SideLength * (SnakeTile_SideLength + 1)) / 2};
inline constexpr std::array<uint8_t, SNAKETILE_DATALENGTH> Snake_HEAD_UP_Data{
    /* clang-format off */
    embp::pfold(BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK, 0,
                BLACK, BLACK, SKIN, SKIN, SKIN, BLACK, BLACK,0,
                BLACK, SKIN,  EYE, SKIN,  EYE, SKIN, BLACK,0,
                BLACK, SKIN,  EYE, SKIN,  EYE, SKIN, BLACK,0,
                BLACK, SKIN, SKIN, SKIN, SKIN, SKIN, BLACK,0,
                BLACK, SKIN, SKIN, SKIN, SKIN, SKIN, BLACK,0,
                BLACK, SKIN, SKIN, SKIN, SKIN, SKIN, BLACK,0)
    /* clang-format on */
};
inline constexpr std::array<uint8_t, SNAKETILE_DATALENGTH> Snake_HEAD_DOWN_Data{
    /* clang-format off */
    embp::pfold(BLACK, SKIN, SKIN, SKIN, SKIN, SKIN, BLACK,0,
                BLACK, SKIN, SKIN, SKIN, SKIN, SKIN, BLACK,0,
                BLACK, SKIN, SKIN, SKIN, SKIN, SKIN, BLACK,0,
                BLACK, SKIN,  EYE, SKIN,  EYE, SKIN, BLACK,0,
                BLACK, SKIN,  EYE, SKIN,  EYE, SKIN, BLACK,0,
                BLACK, BLACK, SKIN, SKIN, SKIN, BLACK, BLACK,0,
                BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,0)
    /* clang-format on */
};
inline constexpr std::array<uint8_t, SNAKETILE_DATALENGTH>
    Snake_HEAD_RIGHT_Data{
        /* clang-format off */
    embp::pfold(BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,0,
                SKIN, SKIN, SKIN, SKIN, SKIN, BLACK, BLACK,0,
                SKIN, SKIN, SKIN,  EYE,  EYE, SKIN, BLACK,0,
                SKIN, SKIN, SKIN, SKIN, SKIN, SKIN, BLACK,0,
                SKIN, SKIN, SKIN,  EYE,  EYE, SKIN, BLACK,0,
                SKIN, SKIN, SKIN, SKIN, SKIN, BLACK, BLACK,0,
                BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,0)
        /* clang-format on */
    };
inline constexpr std::array<uint8_t, SNAKETILE_DATALENGTH> Snake_HEAD_LEFT_Data{
    /* clang-format off */
    embp::pfold(BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,0,
                BLACK, BLACK, SKIN, SKIN, SKIN, SKIN, SKIN,0,
                BLACK, SKIN,  EYE,  EYE, SKIN, SKIN, SKIN,0,
                BLACK, SKIN, SKIN, SKIN, SKIN, SKIN, SKIN,0,
                BLACK, SKIN,  EYE,  EYE, SKIN, SKIN, SKIN,0,
                BLACK, BLACK, SKIN, SKIN, SKIN, SKIN, SKIN,0,
                BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,0)
    /* clang-format on */
};

/* =====================================================================
                       _____     _ _
                      |_   _|_ _(_) |
                        | |/ _` | | |
                        | | (_| | | |
                        |_|\__,_|_|_|

 * ===================================================================== */
inline constexpr std::array<uint8_t, SNAKETILE_DATALENGTH> Snake_TAIL_UP_Data{
    /* clang-format off */
        embp::pfold(
            BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,0,
            BLACK, BLACK, SKIN, SKIN, SKIN, BLACK,BLACK,0,
            BLACK, TAIL_DIM, TAIL_BRIGHT, TAIL_BRIGHT, TAIL_BRIGHT, TAIL_DIM, BLACK,0,
            BLACK, SKIN, SKIN, SKIN, SKIN, SKIN,BLACK,0,
            BLACK, TAIL_DIM, TAIL_BRIGHT, TAIL_BRIGHT, TAIL_BRIGHT, TAIL_DIM, BLACK,0,
            BLACK, SKIN, SKIN, SKIN, SKIN, SKIN,BLACK,0,
            BLACK, SKIN, SKIN, SKIN, SKIN, SKIN,BLACK,0)
    /* clang-format on */
};
inline constexpr std::array<uint8_t, SNAKETILE_DATALENGTH> Snake_TAIL_DOWN_Data{
    /* clang-format off */
        embp::pfold(
            BLACK, SKIN, SKIN, SKIN, SKIN, SKIN,BLACK,0,
            BLACK, SKIN, SKIN, SKIN, SKIN, SKIN,BLACK,0,
            BLACK, TAIL_DIM, TAIL_BRIGHT, TAIL_BRIGHT, TAIL_BRIGHT, TAIL_DIM, BLACK,0,
            BLACK, SKIN, SKIN, SKIN, SKIN, SKIN,BLACK,0,
            BLACK, TAIL_DIM, TAIL_BRIGHT, TAIL_BRIGHT, TAIL_BRIGHT, TAIL_DIM, BLACK,0,
            BLACK, BLACK, SKIN, SKIN, SKIN, BLACK,BLACK,0,
            BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,0)
    /* clang-format on */
};
inline constexpr std::array<uint8_t, SNAKETILE_DATALENGTH> Snake_TAIL_LEFT_Data{
    /* clang-format off */
        embp::pfold(
            BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK,0,
            BLACK,  BLACK, TAIL_DIM, SKIN, TAIL_DIM, SKIN, SKIN,0,
            BLACK,  SKIN, TAIL_BRIGHT, SKIN, TAIL_BRIGHT, SKIN, SKIN,0,
            BLACK,  SKIN, TAIL_BRIGHT, SKIN, TAIL_BRIGHT, SKIN, SKIN,0,
            BLACK,  SKIN, TAIL_BRIGHT, SKIN, TAIL_BRIGHT, SKIN, SKIN,0,
            BLACK,  BLACK, TAIL_DIM, SKIN, TAIL_DIM, SKIN, SKIN,0,
            BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK,0)
    /* clang-format on */
};
inline constexpr std::array<uint8_t, SNAKETILE_DATALENGTH>
    Snake_TAIL_RIGHT_Data{
        /* clang-format off */
        embp::pfold(
            BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK,0,
            SKIN, SKIN, TAIL_DIM, SKIN, TAIL_DIM, BLACK, BLACK,0,
            SKIN, SKIN, TAIL_BRIGHT, SKIN, TAIL_BRIGHT, SKIN, BLACK,0,
            SKIN, SKIN, TAIL_BRIGHT, SKIN, TAIL_BRIGHT, SKIN, BLACK,0,
            SKIN, SKIN, TAIL_BRIGHT, SKIN, TAIL_BRIGHT, SKIN, BLACK,0,
            SKIN, SKIN, TAIL_DIM, SKIN, TAIL_DIM, BLACK, BLACK,0,
            BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK,0)
        /* clang-format on */
    };

/* =====================================================================
                     ____            _
                    | __ )  ___   __| |_   _
                    |  _ \ / _ \ / _` | | | |
                    | |_) | (_) | (_| | |_| |
                    |____/ \___/ \__,_|\__, |
                                       |___/

            Up, down, left, right
 * ===================================================================== */
inline constexpr std::array<uint8_t, SNAKETILE_DATALENGTH> Snake_BODY_UP_Data{
    /* clang-format off */
        embp::pfold(
            BLACK, SKIN, SKINSHN, SKIN, SKIN, SDWSKIN, BLACK,0,
            BLACK, SKIN, SKINSHN, SKIN, SKIN, SDWSKIN, BLACK,0,
            BLACK, SKIN, SKINSHN, SKIN, SKIN, SDWSKIN, BLACK,0,
            BLACK, SKIN, SKINSHN, SKIN, SKIN, SDWSKIN, BLACK,0,
            BLACK, SKIN, SKINSHN, SKIN, SKIN, SDWSKIN, BLACK,0,
            BLACK, SKIN, SKINSHN, SKIN, SKIN, SDWSKIN, BLACK,0,
            BLACK, SKIN, SKINSHN, SKIN, SKIN, SDWSKIN, BLACK,0)
    /* clang-format on */
};
inline constexpr std::array<uint8_t, SNAKETILE_DATALENGTH> Snake_BODY_DOWN_Data{
    /* clang-format off */
        embp::pfold(
            BLACK, SDWSKIN, SKIN, SKIN, SKINSHN, SKIN, BLACK,0,
            BLACK, SDWSKIN, SKIN, SKIN, SKINSHN, SKIN, BLACK,0,
            BLACK, SDWSKIN, SKIN, SKIN, SKINSHN, SKIN, BLACK,0,
            BLACK, SDWSKIN, SKIN, SKIN, SKINSHN, SKIN, BLACK,0,
            BLACK, SDWSKIN, SKIN, SKIN, SKINSHN, SKIN, BLACK,0,
            BLACK, SDWSKIN, SKIN, SKIN, SKINSHN, SKIN, BLACK,0,
            BLACK, SDWSKIN, SKIN, SKIN, SKINSHN, SKIN, BLACK,0)
    /* clang-format on */
};
inline constexpr std::array<uint8_t, SNAKETILE_DATALENGTH> Snake_BODY_LEFT_Data{
    /* clang-format off */
        embp::pfold(
            BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK,0,
            SDWSKIN, SDWSKIN, SDWSKIN, SDWSKIN, SDWSKIN, SDWSKIN, SDWSKIN,0,
            SKIN, SKIN, SKIN, SKIN, SKIN, SKIN, SKIN,0,
            SKIN, SKIN, SKIN, SKIN, SKIN, SKIN, SKIN,0,
            SKINSHN, SKINSHN, SKINSHN, SKINSHN, SKINSHN, SKINSHN, SKINSHN,0,
            SKIN, SKIN, SKIN, SKIN, SKIN, SKIN, SKIN,0,
            BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK,0)
    /* clang-format on */
};
inline constexpr std::array<uint8_t, SNAKETILE_DATALENGTH>
    Snake_BODY_RIGHT_Data{
        /* clang-format off */
        embp::pfold(
            BLACK,   BLACK,   BLACK,   BLACK,   BLACK,   BLACK,   BLACK,0,
            SKIN,    SKIN,    SKIN,    SKIN,    SKIN,    SKIN,    SKIN,0,
            SKINSHN, SKINSHN, SKINSHN, SKINSHN, SKINSHN, SKINSHN, SKINSHN,0,
            SKIN,    SKIN,    SKIN,    SKIN,    SKIN,    SKIN,    SKIN,0,
            SKIN,    SKIN,    SKIN,    SKIN,    SKIN,    SKIN,    SKIN,0,
            SDWSKIN, SDWSKIN, SDWSKIN, SDWSKIN, SDWSKIN, SDWSKIN, SDWSKIN,0,
            BLACK,   BLACK,   BLACK,   BLACK,   BLACK,   BLACK,   BLACK,0)
        /* clang-format on */
    };

/* =====================================================================
                     ____            _
                    | __ )  ___   __| |_   _
                    |  _ \ / _ \ / _` | | | |
                    | |_) | (_) | (_| | |_| |
                    |____/ \___/ \__,_|\__, |
                                       |___/

            Up-left, Left-up, Up-right, Right-up
            previous-next ordering throughout
 * ===================================================================== */
inline constexpr std::array<uint8_t, SNAKETILE_DATALENGTH>
    Snake_CURVE_UPLEFT_Data{
        /* clang-format off */
        embp::pfold(
            BLACK,   SKIN,    SKINSHN, SKIN,    SKIN,    SDWSKIN, BLACK,0,
            SKIN,    SKIN,    SKINSHN, SKIN,    SKIN,    SDWSKIN, BLACK,0,
            SKINSHN, SKINSHN, SKINSHN, SKIN,    SKIN,    SDWSKIN, BLACK,0,
            SKIN,    SKIN,    SKIN,    SKIN,    SKIN,    SDWSKIN, BLACK,0,
            SKIN,    SKIN,    SKIN,    SKIN,    SKIN,    SDWSKIN, BLACK,0,
            SDWSKIN, SDWSKIN, SDWSKIN, SDWSKIN, SDWSKIN, BLACK,   BLACK,0,
            BLACK,   BLACK,   BLACK,   BLACK,   BLACK,   BLACK,   BLACK,0)
        /* clang-format on */
    };
inline constexpr std::array<uint8_t, SNAKETILE_DATALENGTH>
    Snake_CURVE_LEFTUP_Data{
        /* clang-format off */
        embp::pfold(
            BLACK,   SDWSKIN, SKIN,    SKIN,    SKINSHN, SKIN,  BLACK,0,
            SDWSKIN, SDWSKIN, SKIN,    SKIN,    SKINSHN, SKIN,  BLACK,0,
            SKIN,    SKIN,    SKIN,    SKIN,    SKINSHN, SKIN,  BLACK,0,
            SKIN,    SKIN,    SKIN,    SKINSHN, SKINSHN, SKIN,  BLACK,0,
            SKINSHN, SKINSHN, SKINSHN, SKINSHN, SKIN,    BLACK, BLACK,0,
            SKIN,    SKIN,    SKIN,    SKIN,    SKIN,    SKIN,  BLACK,0,
            BLACK,   BLACK,   BLACK,   BLACK,   BLACK,   BLACK, BLACK,0)
        /* clang-format on */
    };
inline constexpr std::array<uint8_t, SNAKETILE_DATALENGTH>
    Snake_CURVE_UPRIGHT_Data{
        /* clang-format off */
        embp::pfold(
            BLACK, SKIN,  SKINSHN, SKIN,    SKIN,    SDWSKIN, BLACK,0,
            BLACK, SKIN,  SKINSHN, SKIN,    SKIN,    SDWSKIN, SDWSKIN,0,
            BLACK, SKIN,  SKINSHN, SKIN,    SKIN,    SKIN,    SKIN,0,
            BLACK, SKIN,  SKINSHN, SKINSHN, SKIN,    SKIN,    SKIN,0,
            BLACK, SKIN,  SKIN,    SKINSHN, SKINSHN, SKINSHN, SKINSHN,0,
            BLACK, BLACK, SKIN,    SKIN,    SKIN,    SKIN,    SKIN,0,
            BLACK, BLACK, BLACK,   BLACK,   BLACK,   BLACK,   BLACK,0)
        /* clang-format on */
    };
inline constexpr std::array<uint8_t, SNAKETILE_DATALENGTH>
    Snake_CURVE_RIGHTUP_Data{
        /* clang-format off */
        embp::pfold(
            BLACK, SDWSKIN,  SKIN,    SKIN,    SKINSHN, SKIN,    BLACK,0,
            BLACK, SDWSKIN,  SKIN,    SKIN,    SKINSHN, SKIN,    SKIN,0,
            BLACK, SDWSKIN,  SKIN,    SKIN,    SKINSHN, SKINSHN, SKINSHN,0,
            BLACK, SDWSKIN,  SKIN,    SKIN,    SKIN,    SKIN,    SKIN,0,
            BLACK, SDWSKIN,  SKIN,    SKIN,    SKIN,    SKIN,    SKIN,0,
            BLACK, BLACK,    SDWSKIN, SDWSKIN, SDWSKIN, SDWSKIN, SDWSKIN,0,
            BLACK, BLACK,    BLACK,   BLACK,   BLACK,   BLACK,   BLACK,0)
        /* clang-format on */
    };

/* =====================================================================
                     ____            _
                    | __ )  ___   __| |_   _
                    |  _ \ / _ \ / _` | | | |
                    | |_) | (_) | (_| | |_| |
                    |____/ \___/ \__,_|\__, |
                                       |___/

            Down-left, Left-down, Down-right, Right-down
            previous-next ordering throughout
 * ===================================================================== */
inline constexpr std::array<uint8_t, SNAKETILE_DATALENGTH>
    Snake_CURVE_DOWNLEFT_Data{
        /* clang-format off */
        embp::pfold(
            BLACK,   BLACK,   BLACK,   BLACK,   BLACK,   BLACK, BLACK,0,
            SKIN,    SKIN,    SKIN,    SKIN,    SKIN,    BLACK,  BLACK,0,
            SKINSHN, SKINSHN, SKINSHN, SKINSHN, SKIN,    SKIN,  BLACK,0,
            SKIN,    SKIN,    SKIN,    SKINSHN, SKINSHN, SKIN,  BLACK,0,
            SKIN,    SKIN,    SKIN,    SKIN,    SKINSHN, SKIN,  BLACK,0,
            SDWSKIN, SDWSKIN, SKIN,    SKIN,    SKINSHN, SKIN,  BLACK,0,
            BLACK,   SDWSKIN, SKIN,    SKIN,    SKINSHN, SKIN,  BLACK,0)
        /* clang-format on */
    };
inline constexpr std::array<uint8_t, SNAKETILE_DATALENGTH>
    Snake_CURVE_LEFTDOWN_Data{
        /* clang-format off */
        embp::pfold(
            BLACK,   BLACK,   BLACK,   BLACK,   BLACK,   BLACK,   BLACK,0,
            SDWSKIN, SDWSKIN, SDWSKIN, SDWSKIN, SDWSKIN, BLACK,   BLACK,0,
            SKIN,    SKIN,    SKIN,    SKIN,    SKIN,    SDWSKIN, BLACK,0,
            SKIN,    SKIN,    SKIN,    SKIN,    SKIN,    SDWSKIN, BLACK,0,
            SKINSHN, SKINSHN, SKINSHN, SKIN,    SKIN,    SDWSKIN, BLACK,0,
            SKIN,    SKIN,    SKINSHN, SKIN,    SKIN,    SDWSKIN, BLACK,0,
            BLACK,   SKIN,    SKINSHN, SKIN,    SKIN,    SDWSKIN, BLACK,0)
        /* clang-format on */
    };
inline constexpr std::array<uint8_t, SNAKETILE_DATALENGTH>
    Snake_CURVE_DOWNRIGHT_Data{
        /* clang-format off */
        embp::pfold(
            BLACK, BLACK,    BLACK,   BLACK,   BLACK,   BLACK,   BLACK,0,
            BLACK, BLACK,    SDWSKIN, SDWSKIN, SDWSKIN, SDWSKIN, SDWSKIN,0,
            BLACK, SDWSKIN,  SKIN,    SKIN,    SKIN,    SKIN,    SKIN,0,
            BLACK, SDWSKIN,  SKIN,    SKIN,    SKIN,    SKIN,    SKIN,0,
            BLACK, SDWSKIN,  SKIN,    SKIN,    SKINSHN, SKINSHN, SKINSHN,0,
            BLACK, SDWSKIN,  SKIN,    SKIN,    SKINSHN, SKIN,    SKIN,0,
            BLACK, SDWSKIN,  SKIN,    SKIN,    SKINSHN, SKIN,    BLACK,0)
        /* clang-format on */
    };
inline constexpr std::array<uint8_t, SNAKETILE_DATALENGTH>
    Snake_CURVE_RIGHTDOWN_Data{
        /* clang-format off */
        embp::pfold(
            BLACK, BLACK, BLACK,   BLACK,   BLACK,   BLACK,   BLACK,0,
            BLACK, BLACK, SKIN,    SKIN,    SKIN,    SKIN,    SKIN,0,
            BLACK, SKIN,  SKIN,    SKINSHN, SKINSHN, SKINSHN, SKINSHN,0,
            BLACK, SKIN,  SKINSHN, SKINSHN, SKIN,    SKIN,    SKIN,0,
            BLACK, SKIN,  SKINSHN, SKIN,    SKIN,    SKIN,    SKIN,0,
            BLACK, SKIN,  SKINSHN, SKIN,    SKIN,    SDWSKIN, SDWSKIN,0,
            BLACK, SKIN,  SKINSHN, SKIN,    SKIN,    SDWSKIN, BLACK,0)
        /* clang-format on */
    };

enum struct SnakeBodyPart : uint8_t {
  HEAD_UP = 0x00,
  HEAD_DOWN,
  HEAD_LEFT,
  HEAD_RIGHT,
  TAIL_UP,
  TAIL_DOWN,
  TAIL_LEFT,
  TAIL_RIGHT,
  BODY_UP,
  BODY_DOWN,
  BODY_LEFT,
  BODY_RIGHT,
  BODY_UPLEFT,
  BODY_LEFTUP,
  BODY_UPRIGHT,
  BODY_RIGHTUP,
  BODY_DOWNLEFT,
  BODY_LEFTDOWN,
  BODY_DOWNRIGHT,
  BODY_RIGHTDOWN,
  end_item
};

inline constexpr std::array SnakeTiles{
    /* Head */
    screen::Tile{.side_length = SnakeTile_SideLength,
                 .format = TILE_FORMAT,
                 .data = std::data(Snake_HEAD_UP_Data)},
    screen::Tile{.side_length = SnakeTile_SideLength,
                 .format = TILE_FORMAT,
                 .data = std::data(Snake_HEAD_DOWN_Data)},
    screen::Tile{.side_length = SnakeTile_SideLength,
                 .format = TILE_FORMAT,
                 .data = std::data(Snake_HEAD_LEFT_Data)},
    screen::Tile{.side_length = SnakeTile_SideLength,
                 .format = TILE_FORMAT,
                 .data = std::data(Snake_HEAD_RIGHT_Data)},

    /* Tail */
    screen::Tile{.side_length = SnakeTile_SideLength,
                 .format = TILE_FORMAT,
                 .data = std::data(Snake_TAIL_UP_Data)},
    screen::Tile{.side_length = SnakeTile_SideLength,
                 .format = TILE_FORMAT,
                 .data = std::data(Snake_TAIL_DOWN_Data)},
    screen::Tile{.side_length = SnakeTile_SideLength,
                 .format = TILE_FORMAT,
                 .data = std::data(Snake_TAIL_LEFT_Data)},
    screen::Tile{.side_length = SnakeTile_SideLength,
                 .format = TILE_FORMAT,
                 .data = std::data(Snake_TAIL_RIGHT_Data)},

    /* Body */
    screen::Tile{.side_length = SnakeTile_SideLength,
                 .format = TILE_FORMAT,
                 .data = std::data(Snake_BODY_UP_Data)},
    screen::Tile{.side_length = SnakeTile_SideLength,
                 .format = TILE_FORMAT,
                 .data = std::data(Snake_BODY_DOWN_Data)},
    screen::Tile{.side_length = SnakeTile_SideLength,
                 .format = TILE_FORMAT,
                 .data = std::data(Snake_BODY_LEFT_Data)},
    screen::Tile{.side_length = SnakeTile_SideLength,
                 .format = TILE_FORMAT,
                 .data = std::data(Snake_BODY_RIGHT_Data)},
    screen::Tile{.side_length = SnakeTile_SideLength,
                 .format = TILE_FORMAT,
                 .data = std::data(Snake_CURVE_UPLEFT_Data)},
    screen::Tile{.side_length = SnakeTile_SideLength,
                 .format = TILE_FORMAT,
                 .data = std::data(Snake_CURVE_LEFTUP_Data)},
    screen::Tile{.side_length = SnakeTile_SideLength,
                 .format = TILE_FORMAT,
                 .data = std::data(Snake_CURVE_UPRIGHT_Data)},
    screen::Tile{.side_length = SnakeTile_SideLength,
                 .format = TILE_FORMAT,
                 .data = std::data(Snake_CURVE_RIGHTUP_Data)},
    screen::Tile{.side_length = SnakeTile_SideLength,
                 .format = TILE_FORMAT,
                 .data = std::data(Snake_CURVE_DOWNLEFT_Data)},
    screen::Tile{.side_length = SnakeTile_SideLength,
                 .format = TILE_FORMAT,
                 .data = std::data(Snake_CURVE_LEFTDOWN_Data)},
    screen::Tile{.side_length = SnakeTile_SideLength,
                 .format = TILE_FORMAT,
                 .data = std::data(Snake_CURVE_DOWNRIGHT_Data)},
    screen::Tile{.side_length = SnakeTile_SideLength,
                 .format = TILE_FORMAT,
                 .data = std::data(Snake_CURVE_RIGHTDOWN_Data)},
};

static_assert(static_cast<uint8_t>(SnakeBodyPart::end_item) ==
              std::size(SnakeTiles));

[[nodiscard]] constexpr const screen::Tile &
to_snake_tile(SnakeBodyPart part) noexcept {
  const auto idx{static_cast<uint32_t>(part)};
  return snake::SnakeTiles[idx];
}

inline constexpr size_t AppleTile_SideLength{GRID_SPACE_PIX};
inline constexpr auto Apple_Tile_Data{
    /* clang-format off */
        embp::pfold(
            BLACK, BLACK, BLACK, BLACK, GREEN, BLACK, BLACK,0,
            BLACK, BLACK, BLACK, GREEN, BLACK, BLACK, BLACK,0,
            BLACK,   YEL,   RED, DIMRED,  RED,   RED, BLACK,0,
            BLACK,   RED,   YEL,   RED,   RED, BLACK, BLACK,0,
            BLACK,   RED,   YEL,   RED,   RED,   RED, BLACK,0,
            BLACK,   RED,   RED,   RED,   RED,   RED, BLACK,0,
            BLACK, BLACK,   RED,   RED,   RED, BLACK, BLACK,0)
    /* clang-format on */
};
inline constexpr screen::Tile AppleTile{.side_length = AppleTile_SideLength,
                                        .format = TILE_FORMAT,
                                        .data = std::data(Apple_Tile_Data)};

inline constexpr size_t BackgroundTile_SideLength{GRID_SPACE_PIX};
inline constexpr auto Background_Tile_Data{
    /* clang-format off */
        embp::pfold(
            BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK,0,
            BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK,0,
            BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK,0,
            BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK,0,
            BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK,0,
            BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK,0,
            BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK,0)
    /* clang-format on */
};
inline constexpr screen::Tile BackgroundTile{
    .side_length = BackgroundTile_SideLength,
    .format = TILE_FORMAT,
    .data = std::data(Background_Tile_Data)};
} // namespace snake

#endif