#if !defined(REVENGE_TILES_HPP)
#define REVENGE_TILES_HPP

#include "common/utilities.hpp"
#include "embp/constexpr_numeric.hpp"
#include "revenge_defs.hpp"
#include "screen/screen.hpp"

namespace revenge {

inline constexpr screen::Format VIDEO_FORMAT{screen::Format::RGB565_LUT4};

inline constexpr std::array Palette{
    /* clang-format off */
    screen::Clut{.r = 0, .g = 0, .b = 0},           /* black */
    screen::Clut{.r = 255, .g = 0, .b = 0},         /* red */
    screen::Clut{.r = 0, .g = 245, .b = 0},         /* green */
    screen::Clut{.r = 0, .g = 0, .b = 255},         /* blue */
    screen::Clut{.r = 0, .g = 220, .b = 220},       /* cyan */
    screen::Clut{.r = 220, .g = 220, .b = 0},       /* yellow */
    screen::Clut{.r = 245, .g = 0, .b = 245},       /* magenta */
    screen::Clut{.r = 100, .g = 100, .b = 100},     /* dark grey */

    screen::Clut{.r = 255, .g = 255, .b = 255},     /* white */
    screen::Clut{.r = 255, .g = 128, .b = 128},     /* light red */
    screen::Clut{.r = 245, .g = 255, .b = 245},     /* light green */
    screen::Clut{.r = 128, .g = 128, .b = 255},     /* light blue */
    screen::Clut{.r = 200, .g = 255, .b = 255},     /* light cyan */
    screen::Clut{.r = 255, .g = 255, .b = 200},     /* light yellow */
    screen::Clut{.r = 255, .g = 128, .b = 255},     /* light magenta */
    screen::Clut{.r = 200, .g = 200, .b = 200},     /* light grey */
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

inline constexpr uint8_t BKGRND{YELLOW};
inline constexpr size_t BTLEN{(PIXELS_PER_GRID / 2) * PIXELS_PER_GRID};

/* tile data */

// clang-format off
inline constexpr auto background_tile{
    embp::filled<uint8_t, BTLEN>(YELLOW << 4 | YELLOW)};

inline constexpr auto block_tile{
    embp::concat(
        embp::pfold(LGREEN, LGREEN,  LGREY, LGREEN, LGREEN, LGREEN,  LGREY, LGREEN, LGREEN, LGREEN),
        embp::pfold(LGREEN,  GREEN,  LGREY,  GREEN,  GREEN,  GREEN,  LGREY,  GREEN,  GREEN, DRKGRY),
        embp::pfold( LGREY,  GREEN,  LGREY,  GREEN,  GREEN,  LGREY,  GREEN,  LGREY,  GREEN, DRKGRY),
        embp::pfold(LGREEN,  LGREY,  GREEN,  LGREY,  LGREY,  GREEN,  GREEN,  LGREY,  GREEN, DRKGRY),
        embp::pfold(LGREEN,  GREEN,  LGREY,  LGREY,  GREEN,  GREEN,  GREEN,  LGREY,  GREEN, DRKGRY),
        embp::pfold(LGREEN,  GREEN,  LGREY,  GREEN,  GREEN,  GREEN,  GREEN,  GREEN,  LGREY, DRKGRY),
        embp::pfold( LGREY,  LGREY,  LGREY,  GREEN,  GREEN,  LGREY,  LGREY,  LGREY,  LGREY,  LGREY),
        embp::pfold(LGREEN,  GREEN,  LGREY,  LGREY,  LGREY,  GREEN,  GREEN,  LGREY,  GREEN, DRKGRY),
        embp::pfold(LGREEN,  GREEN,  GREEN,  LGREY,  GREEN,  GREEN,  LGREY,  GREEN,  GREEN, DRKGRY),
        embp::pfold(DRKGRY, DRKGRY, DRKGRY,  LGREY, DRKGRY, DRKGRY,  LGREY, DRKGRY, DRKGRY, DRKGRY)
    )
};

inline constexpr auto trap_tile{
    embp::filled<uint8_t, BTLEN>(LGREY << 4 | LGREY)};

inline constexpr auto hole_tile{
    embp::concat(
        embp::pfold( BKGRND, BKGRND, BKGRND, BKGRND, BKGRND, BKGRND, BKGRND, BKGRND, BKGRND, BKGRND ),
        embp::pfold( BKGRND, BKGRND, BKGRND, BKGRND, BKGRND, BKGRND, BKGRND, BKGRND, BKGRND, BKGRND ),
        embp::pfold( BKGRND, BKGRND, BKGRND, BKGRND, BKGRND, BKGRND, BKGRND, BKGRND, BKGRND, BKGRND ),
        embp::pfold( BKGRND, BKGRND, BKGRND, BKGRND, BKGRND, BKGRND, BKGRND, BKGRND, BKGRND, BKGRND ),
        embp::pfold( BKGRND, BKGRND, DRKGRY, DRKGRY, DRKGRY, DRKGRY, DRKGRY, DRKGRY, BKGRND, BKGRND ),
        embp::pfold( BKGRND, DRKGRY, DRKGRY, DRKGRY, DRKGRY, DRKGRY, DRKGRY, DRKGRY, DRKGRY, BKGRND ),
        embp::pfold( BKGRND, BKGRND, DRKGRY, DRKGRY, DRKGRY, DRKGRY, DRKGRY, DRKGRY, BKGRND, BKGRND ),
        embp::pfold( BKGRND, BKGRND, BKGRND, DRKGRY, DRKGRY, DRKGRY, DRKGRY, BKGRND, BKGRND, BKGRND ),
        embp::pfold( BKGRND, BKGRND, BKGRND, BKGRND, BKGRND, BKGRND, BKGRND, BKGRND, BKGRND, BKGRND ),
        embp::pfold( BKGRND, BKGRND, BKGRND, BKGRND, BKGRND, BKGRND, BKGRND, BKGRND, BKGRND, BKGRND )
    )
};
inline constexpr auto nonmoveblock_tile{
    embp::concat(
        embp::pfold(  LCYAN,  LCYAN,  LCYAN,  LCYAN,  LCYAN,  LCYAN,  LCYAN,  LCYAN,  LCYAN,  LCYAN),
        embp::pfold(  LCYAN,  LGREY,  LGREY,  LGREY,  LGREY,  LGREY,  LGREY,  LGREY,  LGREY, DRKGRY),
        embp::pfold(  LCYAN,  LGREY,  LCYAN,  LCYAN,  LCYAN,  LCYAN,  LCYAN,  LCYAN,  LGREY, DRKGRY),
        embp::pfold(  LCYAN,  LGREY,  LCYAN,  LGREY,  LGREY,  LGREY,  LGREY, DRKGRY,  LGREY, DRKGRY),
        embp::pfold(  LCYAN,  LGREY,  LCYAN,  LGREY,  LCYAN,  LGREY,  LGREY, DRKGRY,  LGREY, DRKGRY),
        embp::pfold(  LCYAN,  LGREY,  LCYAN,  LGREY,  LGREY, DRKGRY,  LGREY, DRKGRY,  LGREY, DRKGRY),
        embp::pfold(  LCYAN,  LGREY,  LCYAN,  LGREY,  LGREY,  LGREY,  LGREY, DRKGRY,  LGREY, DRKGRY),
        embp::pfold(  LCYAN,  LGREY, DRKGRY, DRKGRY, DRKGRY, DRKGRY, DRKGRY, DRKGRY,  LGREY, DRKGRY),
        embp::pfold(  LCYAN,  LGREY,  LGREY,  LGREY,  LGREY,  LGREY,  LGREY,  LGREY,  LGREY, DRKGRY),
        embp::pfold( DRKGRY, DRKGRY, DRKGRY, DRKGRY, DRKGRY, DRKGRY, DRKGRY, DRKGRY, DRKGRY, DRKGRY)
    )

};

inline constexpr std::array CAT_STANDING_DATA { embp::pfold(
    BKGRND,BKGRND,DRKGRY,BKGRND,BKGRND,BKGRND,DRKGRY,BKGRND,DRKGRY,BKGRND,
    BKGRND,BKGRND,LYELLOW,DRKGRY,BKGRND,DRKGRY,LYELLOW,DRKGRY,LYELLOW,BKGRND,
    BKGRND,LYELLOW,DRKGRY,BKGRND,BKGRND,BKGRND,BKGRND,LYELLOW,LYELLOW,DRKGRY,
    BKGRND,BKGRND,LYELLOW,DRKGRY,DRKGRY,DRKGRY,DRKGRY,LYELLOW,LYELLOW,DRKGRY,
    BKGRND,BKGRND,DRKGRY,LYELLOW,LYELLOW,LYELLOW,LYELLOW,LYELLOW,DRKGRY,BKGRND,
    BKGRND,BKGRND,DRKGRY,LYELLOW,LYELLOW,LYELLOW,LYELLOW,LYELLOW,DRKGRY,BKGRND,
    BKGRND,BKGRND,DRKGRY,LYELLOW,LYELLOW,DRKGRY,LYELLOW,DRKGRY,BKGRND,BKGRND,
    BKGRND,DRKGRY,LYELLOW,DRKGRY,DRKGRY,BKGRND,LYELLOW,DRKGRY,BKGRND,BKGRND,
    BKGRND,DRKGRY,LYELLOW,DRKGRY,BKGRND,BKGRND,LYELLOW,DRKGRY,BKGRND,BKGRND,
    BKGRND,BKGRND,BKGRND,BKGRND,BKGRND,BKGRND,BKGRND,BKGRND,BKGRND,BKGRND
)};

inline constexpr std::array CAT_SITTING_DATA { embp::pfold(
    BKGRND,BKGRND,BKGRND,BKGRND,BKGRND,BKGRND,BKGRND,BKGRND,BKGRND,BKGRND,
    BKGRND,BKGRND,BKGRND,BKGRND,BKGRND,BKGRND,DRKGRY,BKGRND,DRKGRY,BKGRND,
    BKGRND,BKGRND,BKGRND,BKGRND,BKGRND,DRKGRY,LYELLOW,DRKGRY,LYELLOW,BKGRND,
    BKGRND,BKGRND,DRKGRY,BKGRND,BKGRND,BKGRND,BKGRND,LYELLOW,LYELLOW,DRKGRY,
    BKGRND,LYELLOW,LYELLOW,DRKGRY,DRKGRY,DRKGRY,DRKGRY,LYELLOW,LYELLOW,DRKGRY,
    BKGRND,LYELLOW,DRKGRY,LYELLOW,LYELLOW,LYELLOW,LYELLOW,LYELLOW,DRKGRY,BKGRND,
    LYELLOW,BKGRND,DRKGRY,LYELLOW,LYELLOW,LYELLOW,LYELLOW,LYELLOW,DRKGRY,BKGRND,
    BKGRND,BKGRND,DRKGRY,LYELLOW,LYELLOW,DRKGRY,LYELLOW,DRKGRY,BKGRND,BKGRND,
    BKGRND,LYELLOW,LYELLOW,DRKGRY,BKGRND,BKGRND,LYELLOW,LYELLOW,DRKGRY,BKGRND,
    BKGRND,LGREY,LGREY,LGREY,BKGRND,BKGRND,LGREY,LGREY,LGREY,BKGRND
)};

inline constexpr auto mouse_tile{
    embp::concat(
        embp::filled<uint8_t, PIXELS_PER_GRID>(BKGRND << 4 | BKGRND),
        embp::pfold( BKGRND,  LGREY, DRKGRY, BKGRND, DRKGRY,  LGREY, BKGRND, BKGRND, BKGRND, BKGRND ),
        embp::pfold( BKGRND, BKGRND,  LGREY,  LGREY, BKGRND, BKGRND, BKGRND, BKGRND, BKGRND, BKGRND ),
        embp::pfold( BKGRND, BKGRND,  LGREY,  LGREY,  LGREY,  BLACK,  LGREY,  BLACK, DRKGRY, BKGRND ),
        embp::pfold( BKGRND,  BLACK,  BLACK, BKGRND,  LGREY,  LGREY,  LGREY,  LGREY, BKGRND, BKGRND ),
        embp::pfold( BKGRND, BKGRND, BKGRND, BKGRND,  LGREY,  LGREY,  LGREY, BKGRND, BKGRND, BKGRND ),
        embp::pfold( BKGRND, BKGRND, BKGRND,  LGREY,  BLACK,  BLACK,  BLACK,  LGREY, BKGRND, BKGRND ),
        embp::pfold( BKGRND, BKGRND,  LGREY,  BLACK, BKGRND, BKGRND, BKGRND,  BLACK,  LGREY, BKGRND ),
        embp::pfold( BKGRND, BKGRND, BKGRND, BKGRND, BKGRND, BKGRND, BKGRND, BKGRND, BKGRND, BKGRND ),
        embp::filled<uint8_t, PIXELS_PER_GRID>(BKGRND << 4 | BKGRND)
    )
};
inline constexpr auto mouse_stuck_tile{
    embp::concat(
        embp::pfold( BKGRND, BKGRND, BKGRND, BKGRND, BKGRND, BKGRND, BKGRND, BKGRND, BKGRND, BKGRND ),
        embp::pfold( BKGRND, BKGRND,  LGREY,  LGREY, BKGRND,  LGREY,  LGREY, BKGRND, BKGRND, BKGRND ),
        embp::pfold( BKGRND, BKGRND, BKGRND,  LGREY,  LGREY,  LGREY, BKGRND, BKGRND, BKGRND, BKGRND ),
        embp::pfold( BKGRND, BKGRND, BKGRND,  LGREY,  LGREY,  LGREY, BKGRND, BKGRND, BKGRND, BKGRND ),
        embp::pfold( BKGRND, BKGRND,  LGREY, DRKGRY,  LGREY, DRKGRY, DRKGRY, DRKGRY, BKGRND, BKGRND ),
        embp::pfold( BKGRND, DRKGRY, DRKGRY, DRKGRY,  LGREY,  LGREY, DRKGRY, DRKGRY, DRKGRY, BKGRND ),
        embp::pfold( BKGRND, BKGRND, DRKGRY,  LGREY,  LGREY,  LGREY,  LGREY, DRKGRY, BKGRND, BKGRND ),
        embp::pfold( BKGRND, BKGRND, BKGRND,  LGREY,  LGREY,  LGREY,  LGREY, BKGRND, BKGRND, BKGRND ),
        embp::pfold( BKGRND, BKGRND, BKGRND, BKGRND, BKGRND, BKGRND, BKGRND, BKGRND, BKGRND, BKGRND ),
        embp::pfold( BKGRND, BKGRND, BKGRND, BKGRND, BKGRND, BKGRND, BKGRND, BKGRND, BKGRND, BKGRND )
    )
};

/* generated by tools/revenge_img_to_cpp.py */
inline constexpr std::array CHEESE_TILE_DATA { embp::pfold(
BKGRND,BKGRND,BKGRND,BKGRND,BKGRND,BKGRND,BKGRND,DRKGRY,DRKGRY,BKGRND,
BKGRND,BKGRND,BKGRND,BKGRND,DRKGRY,DRKGRY,DRKGRY,LYELLOW,LYELLOW,DRKGRY,
BKGRND,BKGRND,DRKGRY,DRKGRY,BKGRND,LYELLOW,LYELLOW,BKGRND,DRKGRY,DRKGRY,
BKGRND,DRKGRY,BKGRND,LYELLOW,LYELLOW,DRKGRY,DRKGRY,DRKGRY,LYELLOW,DRKGRY,
BKGRND,DRKGRY,DRKGRY,DRKGRY,DRKGRY,LYELLOW,LYELLOW,BKGRND,LYELLOW,DRKGRY,
BKGRND,DRKGRY,LYELLOW,LYELLOW,LYELLOW,BKGRND,LYELLOW,LYELLOW,LYELLOW,DRKGRY,
BKGRND,DRKGRY,LYELLOW,BKGRND,LYELLOW,LYELLOW,LYELLOW,BKGRND,LYELLOW,DRKGRY,
BKGRND,DRKGRY,BKGRND,LYELLOW,LYELLOW,LYELLOW,DRKGRY,DRKGRY,DRKGRY,BKGRND,
BKGRND,LGREY,DRKGRY,DRKGRY,DRKGRY,DRKGRY,BKGRND,BKGRND,BKGRND,BKGRND,
BKGRND,BKGRND,BKGRND,BKGRND,BKGRND,BKGRND,BKGRND,BKGRND,BKGRND,BKGRND
)};
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
                                     .data = std::data(CHEESE_TILE_DATA)};
inline constexpr screen::Tile CAT{.side_length = PIXELS_PER_GRID,
                                  .format = VIDEO_FORMAT,
                                  .data = std::data(CAT_STANDING_DATA)};
inline constexpr screen::Tile SITTING_CAT{.side_length = PIXELS_PER_GRID,
                                          .format = VIDEO_FORMAT,
                                          .data = std::data(CAT_SITTING_DATA)};
inline constexpr screen::Tile MOUSE{.side_length = PIXELS_PER_GRID,
                                    .format = VIDEO_FORMAT,
                                    .data = std::data(mouse_tile)};
inline constexpr screen::Tile MOUSE_IN_HOLE{.side_length = PIXELS_PER_GRID,
                                            .format = VIDEO_FORMAT,
                                            .data =
                                                std::data(mouse_stuck_tile)};
} // namespace revenge

#endif
