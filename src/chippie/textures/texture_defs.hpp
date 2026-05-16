#if !defined(TEXTURE_DEFS_HPP)
#define TEXTURE_DEFS_HPP

#include <array>
#include <cstdint>

#include "screen/TileDef.h"
#include "screen/screen.hpp"

namespace chippie
{
constexpr uint8_t TILE_SCREEN_FORMAT{screen::Format::RGB565_LUT4};
constexpr uint8_t TILE_SIDE_LENGTH{24}; /* global for all sprites */

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
} // namespace chippie
#endif