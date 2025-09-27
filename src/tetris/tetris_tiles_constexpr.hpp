#if !defined(TETRIS_TILES_CONSTEXPR_HPP)
#define TETRIS_TILES_CONSTEXPR_HPP

#include <array>
#include <cstddef>
#include <cstdint>

#include "screen/TileDef.h"
#include "screen/constexpr_tile_utils.hpp"
#include "screen/screen_def.h"

#include "../utilities.hpp"
#include "tetris_defs.hpp"

#include "embp/constexpr_numeric.hpp"

namespace tetris {

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
    screen::Clut{.r = 64, .g = 64, .b = 64},        /* dark grey */

    screen::Clut{.r = 255, .g = 255, .b = 255},     /* white */
    screen::Clut{.r = 255, .g = 128, .b = 128},     /* light red */
    screen::Clut{.r = 200, .g = 255, .b = 200},     /* light green */
    screen::Clut{.r = 128, .g = 128, .b = 255},     /* light blue */
    screen::Clut{.r = 200, .g = 255, .b = 255},     /* light cyan */
    screen::Clut{.r = 255, .g = 255, .b = 200},     /* light yellow */
    screen::Clut{.r = 255, .g = 128, .b = 255},     /* light magenta */
    screen::Clut{.r = 128, .g = 128, .b = 128},     /* light grey */
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

inline constexpr size_t TetriminoTile_SideLength{SQUARE_SIZE};
inline constexpr size_t BTLEN{(TetriminoTile_SideLength / 2) *
                              TetriminoTile_SideLength};

inline constexpr auto helper_make_tile(uint8_t darkcolor, uint8_t lightcolor) {
  /* clang-format off */
    return 
        embp::pfold(
            lightcolor,lightcolor,lightcolor,lightcolor,lightcolor,lightcolor,lightcolor,lightcolor,lightcolor,lightcolor,lightcolor,BLACK,
            lightcolor,lightcolor,lightcolor,lightcolor,lightcolor,lightcolor,lightcolor,lightcolor,lightcolor,lightcolor,darkcolor,BLACK,
            lightcolor,lightcolor,lightcolor,lightcolor,lightcolor,lightcolor,lightcolor,lightcolor,lightcolor,darkcolor,darkcolor,BLACK,
            lightcolor,lightcolor,lightcolor,darkcolor,darkcolor,darkcolor,darkcolor,darkcolor,darkcolor,darkcolor,darkcolor,BLACK,
            lightcolor,lightcolor,lightcolor,darkcolor,darkcolor,darkcolor,darkcolor,darkcolor,darkcolor,darkcolor,darkcolor,BLACK,
            lightcolor,lightcolor,lightcolor,darkcolor,darkcolor,darkcolor,darkcolor,darkcolor,darkcolor,darkcolor,darkcolor,BLACK,
            lightcolor,lightcolor,lightcolor,darkcolor,darkcolor,darkcolor,darkcolor,darkcolor,darkcolor,darkcolor,darkcolor,BLACK,
            lightcolor,lightcolor,lightcolor,darkcolor,darkcolor,darkcolor,darkcolor,darkcolor,darkcolor,darkcolor,darkcolor,BLACK,
            lightcolor,lightcolor,darkcolor,darkcolor,darkcolor,darkcolor,darkcolor,darkcolor,darkcolor,darkcolor,darkcolor,BLACK,
            lightcolor,darkcolor,darkcolor,darkcolor,darkcolor,darkcolor,darkcolor,darkcolor,darkcolor,darkcolor,darkcolor,BLACK,
            BLACK,darkcolor,darkcolor,darkcolor,darkcolor,darkcolor,darkcolor,darkcolor,darkcolor,darkcolor,darkcolor,BLACK,
            BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK
        );
  /* clang-format on */
}

inline constexpr std::array Tetrimino_Tile_Data{
    /* clang-format off */
embp::concat(
    /* Blank  */
    embp::filled<uint8_t, BTLEN>(( BLACK<<4 ) | BLACK),
    /* A */
    helper_make_tile(RED, LRED),
    /* B */
    helper_make_tile(MAGENTA, LMAGENTA),
    /* C */
    helper_make_tile(YELLOW, LYELLOW),
    /* D */
    helper_make_tile(GREEN, LGREEN),
    /* E */
    helper_make_tile(BLUE, LBLUE),
    /* F */
    helper_make_tile(DRKGRY, LGREY),
    /* G */
    helper_make_tile(CYAN, LCYAN),
    /* just for clearing the piece preview */
    embp::filled<uint8_t, BTLEN>(( LGREY<<4 ) | LGREY)
)
    /* clang-format on */
};
static_assert(std::size(Tetrimino_Tile_Data) == BTLEN * 9);

inline constexpr std::array TETRIMINO_TILES{
    /* BLANK */ screen::Tile{.side_length = TetriminoTile_SideLength,
                             .format = VIDEO_FORMAT,
                             .data = std::data(Tetrimino_Tile_Data)},
    /* A */
    screen::Tile{.side_length = TetriminoTile_SideLength,
                 .format = VIDEO_FORMAT,
                 .data = std::next(std::data(Tetrimino_Tile_Data), BTLEN)},
    /* B */
    screen::Tile{.side_length = TetriminoTile_SideLength,
                 .format = VIDEO_FORMAT,
                 .data = std::next(std::data(Tetrimino_Tile_Data), 2 * BTLEN)},
    /* C */
    screen::Tile{.side_length = TetriminoTile_SideLength,
                 .format = VIDEO_FORMAT,
                 .data = std::next(std::data(Tetrimino_Tile_Data), 3 * BTLEN)},
    /* D */
    screen::Tile{.side_length = TetriminoTile_SideLength,
                 .format = VIDEO_FORMAT,
                 .data = std::next(std::data(Tetrimino_Tile_Data), 4 * BTLEN)},
    /* E */
    screen::Tile{.side_length = TetriminoTile_SideLength,
                 .format = VIDEO_FORMAT,
                 .data = std::next(std::data(Tetrimino_Tile_Data), 5 * BTLEN)},
    /* F */
    screen::Tile{.side_length = TetriminoTile_SideLength,
                 .format = VIDEO_FORMAT,
                 .data = std::next(std::data(Tetrimino_Tile_Data), 6 * BTLEN)},
    /* G */
    screen::Tile{.side_length = TetriminoTile_SideLength,
                 .format = VIDEO_FORMAT,
                 .data = std::next(std::data(Tetrimino_Tile_Data), 7 * BTLEN)},
    /* PREVIEW */
    screen::Tile{.side_length = TetriminoTile_SideLength,
                 .format = VIDEO_FORMAT,
                 .data = std::next(std::data(Tetrimino_Tile_Data), 8 * BTLEN)},
};
static_assert(std::size(TETRIMINO_TILES) == 9);

} // namespace tetris

#endif