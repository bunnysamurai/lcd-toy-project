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
inline constexpr std::array<screen::Clut, 6> Palette{
    /* clang-format off */
    screen::Clut{.r = 0, .g = 0, .b = 0},                   /* black */
    screen::Clut{.r = 0, .g = 255, .b = 0},            /* snake skin */
    screen::Clut{.r = 250, .g = 250, .b = 127}, /* snake tail, bright */
    screen::Clut{.r = 250>>1, .g = 250>>1, .b = 127>>1}, /* snake tail, dim */
    screen::Clut{.r = 0, .g = 0, .b = 255},             /* blue border, snake eye */
    screen::Clut{.r = 255, .g = 0, .b = 0},             /* red apple */
    /* clang-format on */
};
inline constexpr uint8_t BLACK{0};
inline constexpr uint8_t SKIN{1};
inline constexpr uint8_t TAIL_BRIGHT{2};
inline constexpr uint8_t TAIL_DIM{3};
inline constexpr uint8_t EYE{4};
inline constexpr uint8_t BORDER{EYE};
inline constexpr uint8_t APPLE{5};

inline constexpr screen::Format TILE_FORMAT{screen::Format::RGB565_LUT4};

inline constexpr size_t BorderTile_SideLength{GRID_SPACE_PIX};
inline constexpr std::array<
    uint8_t, (BorderTile_SideLength * (BorderTile_SideLength + 1)) / 2>
    Border_Tile_Data{
        /* clang-format off */
    embp::pfold(BORDER,BORDER,BORDER,BORDER,BORDER,BORDER,BORDER,0,
                BORDER,BORDER,BORDER,BORDER,BORDER,BORDER,BORDER,0,
                BORDER,BORDER,BORDER,BORDER,BORDER,BORDER,BORDER,0,
                BORDER,BORDER,BORDER,BORDER,BORDER,BORDER,BORDER,0,
                BORDER,BORDER,BORDER,BORDER,BORDER,BORDER,BORDER,0,
                BORDER,BORDER,BORDER,BORDER,BORDER,BORDER,BORDER,0, 
                BORDER,BORDER,BORDER,BORDER,BORDER,BORDER,BORDER,0)
        /* clang-format on */
    };
inline constexpr screen::Tile BorderTile{.side_length = BorderTile_SideLength,
                                         .format = TILE_FORMAT,
                                         .data = std::data(Border_Tile_Data)};

inline constexpr size_t SnakeTile_SideLength{GRID_SPACE_PIX};
inline constexpr size_t SNAKETILE_DATALENGTH{
    (SnakeTile_SideLength * (SnakeTile_SideLength + 1)) / 2};
inline constexpr std::array<uint8_t, SNAKETILE_DATALENGTH> Snake_HEAD_UP_Data{
    /* clang-format off */
    embp::pfold(BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK, 0,
                BLACK, SKIN, SKIN, SKIN, SKIN, SKIN, BLACK,0,
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
                BLACK, SKIN, SKIN, SKIN, SKIN, SKIN, BLACK,0,
                BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,0)
    /* clang-format on */
};
inline constexpr std::array<uint8_t, SNAKETILE_DATALENGTH>
    Snake_HEAD_RIGHT_Data{
        /* clang-format off */
    embp::pfold(BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,0,
                SKIN, SKIN, SKIN, SKIN, SKIN, SKIN, BLACK,0,
                SKIN, SKIN, SKIN,  EYE,  EYE, SKIN, BLACK,0,
                SKIN, SKIN, SKIN, SKIN, SKIN, SKIN, BLACK,0,
                SKIN, SKIN, SKIN,  EYE,  EYE, SKIN, BLACK,0,
                SKIN, SKIN, SKIN, SKIN, SKIN, SKIN, BLACK,0,
                BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,0)
        /* clang-format on */
    };
inline constexpr std::array<uint8_t, SNAKETILE_DATALENGTH> Snake_HEAD_LEFT_Data{
    /* clang-format off */
    embp::pfold(BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,0,
                BLACK, SKIN, SKIN, SKIN, SKIN, SKIN, SKIN,0,
                BLACK, SKIN,  EYE,  EYE, SKIN, SKIN, SKIN,0,
                BLACK, SKIN, SKIN, SKIN, SKIN, SKIN, SKIN,0,
                BLACK, SKIN,  EYE,  EYE, SKIN, SKIN, SKIN,0,
                BLACK, SKIN, SKIN, SKIN, SKIN, SKIN, SKIN,0,
                BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,0)
    /* clang-format on */
};
inline constexpr std::array<uint8_t, SNAKETILE_DATALENGTH> Snake_TAIL_UP_Data{
    /* clang-format off */
        embp::pfold(
            BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,0,
            BLACK, SKIN, SKIN, SKIN, SKIN, SKIN,BLACK,0,
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
            BLACK, SKIN, SKIN, SKIN, SKIN, SKIN,BLACK,0,
            BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,0)
    /* clang-format on */
};
inline constexpr std::array<uint8_t, SNAKETILE_DATALENGTH> Snake_TAIL_LEFT_Data{
    /* clang-format off */
        embp::pfold(
            BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK,0,
            BLACK,  SKIN, TAIL_DIM, SKIN, TAIL_DIM, SKIN, SKIN,0,
            BLACK,  SKIN, TAIL_BRIGHT, SKIN, TAIL_BRIGHT, SKIN, SKIN,0,
            BLACK,  SKIN, TAIL_BRIGHT, SKIN, TAIL_BRIGHT, SKIN, SKIN,0,
            BLACK,  SKIN, TAIL_BRIGHT, SKIN, TAIL_BRIGHT, SKIN, SKIN,0,
            BLACK,  SKIN, TAIL_DIM, SKIN, TAIL_DIM, SKIN, SKIN,0,
            BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK,0)
    /* clang-format on */
};
inline constexpr std::array<uint8_t, SNAKETILE_DATALENGTH>
    Snake_TAIL_RIGHT_Data{
        /* clang-format off */
        embp::pfold(
            BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK,0,
            SKIN, SKIN, TAIL_DIM, SKIN, TAIL_DIM, SKIN, BLACK,0,
            SKIN, SKIN, TAIL_BRIGHT, SKIN, TAIL_BRIGHT, SKIN, BLACK,0,
            SKIN, SKIN, TAIL_BRIGHT, SKIN, TAIL_BRIGHT, SKIN, BLACK,0,
            SKIN, SKIN, TAIL_BRIGHT, SKIN, TAIL_BRIGHT, SKIN, BLACK,0,
            SKIN, SKIN, TAIL_DIM, SKIN, TAIL_DIM, SKIN, BLACK,0,
            BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK,0)
        /* clang-format on */
    };
inline constexpr std::array<uint8_t, SNAKETILE_DATALENGTH>
    Snake_BODY_UPDOWN_Data{
        /* clang-format off */
        embp::pfold(
            BLACK, SKIN, SKIN, SKIN, SKIN, SKIN, BLACK,0,
            BLACK, SKIN, SKIN, SKIN, SKIN, SKIN, BLACK,0,
            BLACK, SKIN, SKIN, SKIN, SKIN, SKIN, BLACK,0,
            BLACK, SKIN, SKIN, SKIN, SKIN, SKIN, BLACK,0,
            BLACK, SKIN, SKIN, SKIN, SKIN, SKIN, BLACK,0,
            BLACK, SKIN, SKIN, SKIN, SKIN, SKIN, BLACK,0,
            BLACK, SKIN, SKIN, SKIN, SKIN, SKIN, BLACK,0)
        /* clang-format on */
    };
inline constexpr std::array<uint8_t, SNAKETILE_DATALENGTH>
    Snake_BODY_LEFTRIGHT_Data{
        /* clang-format off */
        embp::pfold(
            BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK,0,
            SKIN, SKIN, SKIN, SKIN, SKIN, SKIN, SKIN,0,
            SKIN, SKIN, SKIN, SKIN, SKIN, SKIN, SKIN,0,
            SKIN, SKIN, SKIN, SKIN, SKIN, SKIN, SKIN,0,
            SKIN, SKIN, SKIN, SKIN, SKIN, SKIN, SKIN,0,
            SKIN, SKIN, SKIN, SKIN, SKIN, SKIN, SKIN,0,
            BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK,0)
        /* clang-format on */
    };
inline constexpr std::array<uint8_t, SNAKETILE_DATALENGTH>
    Snake_CURVE_UPLEFT_Data{
        /* clang-format off */
        embp::pfold(
            BLACK, SKIN, SKIN, SKIN, SKIN, SKIN, BLACK,0,
            SKIN, SKIN, SKIN, SKIN, SKIN, SKIN, BLACK,0,
            SKIN, SKIN, SKIN, SKIN, SKIN, SKIN, BLACK,0,
            SKIN, SKIN, SKIN, SKIN, SKIN, SKIN, BLACK,0,
            SKIN, SKIN, SKIN, SKIN, SKIN, SKIN, BLACK,0,
            SKIN, SKIN, SKIN, SKIN, SKIN, SKIN, BLACK,0,
            BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK,0)
        /* clang-format on */
    };
inline constexpr std::array<uint8_t, SNAKETILE_DATALENGTH>
    Snake_CURVE_UPRIGHT_Data{
        /* clang-format off */
        embp::pfold(
            BLACK, SKIN, SKIN, SKIN, SKIN, SKIN, BLACK,0,
            BLACK, SKIN, SKIN, SKIN, SKIN, SKIN, SKIN,0,
            BLACK, SKIN, SKIN, SKIN, SKIN, SKIN, SKIN,0,
            BLACK, SKIN, SKIN, SKIN, SKIN, SKIN, SKIN,0,
            BLACK, SKIN, SKIN, SKIN, SKIN, SKIN, SKIN,0,
            BLACK, SKIN, SKIN, SKIN, SKIN, SKIN, SKIN,0,
            BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK,0)
        /* clang-format on */
    };
inline constexpr std::array<uint8_t, SNAKETILE_DATALENGTH>
    Snake_CURVE_DOWNLEFT_Data{
        /* clang-format off */
        embp::pfold(
            BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK,0,
            SKIN, SKIN, SKIN, SKIN, SKIN, SKIN, BLACK,0,
            SKIN, SKIN, SKIN, SKIN, SKIN, SKIN, BLACK,0,
            SKIN, SKIN, SKIN, SKIN, SKIN, SKIN, BLACK,0,
            SKIN, SKIN, SKIN, SKIN, SKIN, SKIN, BLACK,0,
            SKIN, SKIN, SKIN, SKIN, SKIN, SKIN, BLACK,0,
            BLACK, SKIN, SKIN, SKIN, SKIN, SKIN, BLACK,0)
        /* clang-format on */
    };
inline constexpr std::array<uint8_t, SNAKETILE_DATALENGTH>
    Snake_CURVE_DOWNRIGHT_Data{
        /* clang-format off */
        embp::pfold(
            BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK,0,
            BLACK, SKIN, SKIN, SKIN, SKIN, SKIN, SKIN,0,
            BLACK, SKIN, SKIN, SKIN, SKIN, SKIN, SKIN,0,
            BLACK, SKIN, SKIN, SKIN, SKIN, SKIN, SKIN,0,
            BLACK, SKIN, SKIN, SKIN, SKIN, SKIN, SKIN,0,
            BLACK, SKIN, SKIN, SKIN, SKIN, SKIN, SKIN,0,
            BLACK, SKIN, SKIN, SKIN, SKIN, SKIN, BLACK,0)
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
  BODY_UPDOWN,
  BODY_LEFTRIGHT,
  BODY_UPLEFT,
  BODY_UPRIGHT,
  BODY_DOWNLEFT,
  BODY_DOWNRIGHT,
  end_item
};

inline constexpr std::array<screen::Tile, 14> SnakeTiles{
    /* Head */
    screen::Tile{.side_length = SnakeTile_SideLength,
                 .format = TILE_FORMAT,
                 .data = std::data(Snake_HEAD_UP_Data)},
    {.side_length = SnakeTile_SideLength,
     .format = TILE_FORMAT,
     .data = std::data(Snake_HEAD_DOWN_Data)},
    {.side_length = SnakeTile_SideLength,
     .format = TILE_FORMAT,
     .data = std::data(Snake_HEAD_LEFT_Data)},
    {.side_length = SnakeTile_SideLength,
     .format = TILE_FORMAT,
     .data = std::data(Snake_HEAD_RIGHT_Data)},

    /* Tail */
    {.side_length = SnakeTile_SideLength,
     .format = TILE_FORMAT,
     .data = std::data(Snake_TAIL_UP_Data)},
    {.side_length = SnakeTile_SideLength,
     .format = TILE_FORMAT,
     .data = std::data(Snake_TAIL_DOWN_Data)},
    {.side_length = SnakeTile_SideLength,
     .format = TILE_FORMAT,
     .data = std::data(Snake_TAIL_LEFT_Data)},
    {.side_length = SnakeTile_SideLength,
     .format = TILE_FORMAT,
     .data = std::data(Snake_TAIL_RIGHT_Data)},

    /* Body */
    {.side_length = SnakeTile_SideLength,
     .format = TILE_FORMAT,
     .data = std::data(Snake_BODY_UPDOWN_Data)},
    {.side_length = SnakeTile_SideLength,
     .format = TILE_FORMAT,
     .data = std::data(Snake_BODY_LEFTRIGHT_Data)},
    {.side_length = SnakeTile_SideLength,
     .format = TILE_FORMAT,
     .data = std::data(Snake_CURVE_UPLEFT_Data)},
    {.side_length = SnakeTile_SideLength,
     .format = TILE_FORMAT,
     .data = std::data(Snake_CURVE_UPRIGHT_Data)},
    {.side_length = SnakeTile_SideLength,
     .format = TILE_FORMAT,
     .data = std::data(Snake_CURVE_DOWNLEFT_Data)},
    {.side_length = SnakeTile_SideLength,
     .format = TILE_FORMAT,
     .data = std::data(Snake_CURVE_DOWNRIGHT_Data)},
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
            APPLE, APPLE, APPLE, APPLE, APPLE, APPLE, APPLE,0,
            APPLE, APPLE, APPLE, APPLE, APPLE, APPLE, APPLE,0,
            APPLE, APPLE, APPLE, APPLE, APPLE, APPLE, APPLE,0,
            APPLE, APPLE, APPLE, APPLE, APPLE, APPLE, APPLE,0,
            APPLE, APPLE, APPLE, APPLE, APPLE, APPLE, APPLE,0,
            APPLE, APPLE, APPLE, APPLE, APPLE, APPLE, APPLE,0,
            APPLE, APPLE, APPLE, APPLE, APPLE, APPLE, APPLE,0)
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