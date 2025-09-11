#if !defined(TETRIS_TILES_CONSTEXPR_HPP)
#define TETRIS_TILES_CONSTEXPR_HPP

#include <array>
#include <cstddef>
#include <cstdint>

#include "screen/TileDef.h"
#include "screen/constexpr_tile_utils.hpp"
#include "screen/screen_def.h"

namespace tetris {

inline constexpr std::array Palette{
    /* clang-format off */
    screen::Clut{.r = 0, .g = 0, .b = 0},           /* black */
    screen::Clut{.r = 255, .g = 0, .b = 0},         /* red */
    screen::Clut{.r = 0, .g = 255, .b = 0},         /* green */
    screen::Clut{.r = 0, .g = 0, .b = 255},         /* blue */
    screen::Clut{.r = 0, .g = 255, .b = 255},       /* cyan */
    screen::Clut{.r = 255, .g = 255, .b = 0},       /* yellow */
    screen::Clut{.r = 255, .g = 255, .b = 0},       /* magenta */
    screen::Clut{.r = 64, .g = 64, .b = 64},        /* dark grey */

    screen::Clut{.r = 255, .g = 255, .b = 255},     /* white */
    screen::Clut{.r = 255, .g = 128, .b = 128},     /* light red */
    screen::Clut{.r = 128, .g = 255, .b = 128},     /* light green */
    screen::Clut{.r = 128, .g = 128, .b = 255},     /* light blue */
    screen::Clut{.r = 128, .g = 255, .b = 255},     /* light cyan */
    screen::Clut{.r = 255, .g = 255, .b = 128},     /* light yellow */
    screen::Clut{.r = 255, .g = 255, .b = 128},     /* light magenta */
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
}

#endif