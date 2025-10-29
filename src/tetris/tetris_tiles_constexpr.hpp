#if !defined(TETRIS_TILES_CONSTEXPR_HPP)
#define TETRIS_TILES_CONSTEXPR_HPP

#include <array>
#include <cstddef>
#include <cstdint>

#include "screen/TileDef.h"
#include "screen/constexpr_tile_utils.hpp"
#include "screen/screen_def.h"

#include "common/utilities.hpp"
#include "tetris_defs.hpp"

#include "embp/constexpr_numeric.hpp"

#include "background_tiles/tiles.hpp"

namespace tetris {

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

// /* and here are the background tiles */
// static constexpr uint32_t BACKGROUND_TILE_SIZE_PIXELS{40};

// inline constexpr std::array LEVEL_BACKGROUND_TILES{
//     screen::Tile{.side_length = BACKGROUND_TILE_SIZE_PIXELS,
//                  .format = VIDEO_FORMAT,
//                  .data = std::data(Background_Tile_Data)}};

/**/
} // namespace tetris

#endif