#if !defined(SNAKE_LEVEL_CONSTEXPR_HPP)
#define SNAKE_LEVEL_CONSTEXPR_HPP

#include <algorithm>
#include <array>

#include "snake_common.hpp"
#include "utilities.hpp"

namespace snake {

/* A snake game play area is always 31x31.  We'll bake this assumption in. */

enum struct StructureType : uint8_t { POINT, LINE, RECT, STRAIGHT_LINE };

/** @brief basic building block of a level
 *
 *  A structure can be:
 *      Point
 *      Line
 *      Rectangle (never filled?)
 */
struct Structure {
  StructureType type : 2;
  uint8_t len : 6; /* 'len' instances of this type */
  const uint8_t *data;
};

/** @brief level descriptor
 *
 *  A level is composed of a number of Structures.
 *
 */
struct Level {
  uint8_t len;
  const Structure *data;
};

struct Point {
  grid_t x;
  grid_t y;
};

[[nodiscard]] constexpr bool operator==(const Point &lhs,
                                        const Point &rhs) noexcept {
  return lhs.x == rhs.x && lhs.y == lhs.y;
}

/** @brief Encode a point
 *
 * @param x column position, in grid-space
 * @param y row position, in grid-space
 */
[[nodiscard]] constexpr std::array<uint8_t, 2> encode_point(grid_t x,
                                                            grid_t y) noexcept {
  return std::array<uint8_t, 2>{x, y};
}
[[nodiscard]] constexpr Point decode_point(const uint8_t *data) noexcept {
  return {.x = data[0], .y = data[1]};
}

struct StraightLine {
  grid_t xs;
  grid_t ys;
  grid_t len : 5;
  uint8_t reserved : 1;
  Direction dir : 2;
};

[[nodiscard]] constexpr bool operator==(const StraightLine &lhs,
                                        const StraightLine &rhs) {
  return lhs.xs == rhs.xs && lhs.ys == rhs.ys && lhs.len == rhs.len &&
         lhs.dir == rhs.dir;
}

/** @brief Encode a "straight" line
 *
 * @param xs start column position, in grid-space
 * @param ys start row position, in grid-space
 * @param dir direction the line runs
 * @param len length of the line
 *
 * @return Byte array, encoding the "straight" line defintion
 */
[[nodiscard]] constexpr std::array<uint8_t, 3>
encode_straight_line(grid_t xs, grid_t ys, Direction dir, grid_t len) noexcept {

  /* we can encode into 5 + 5 + 2 + 5 bits = 17 bits :(
   * So, need 3 bytes
   *   byte[0] = xs
   *   byte[1] = ys
   *   byte[2][7:3] = len
   *   byte[2][1:0] = dir
   */

  return {xs, ys,
          static_cast<uint8_t>(((len & 0b11111) << 3) |
                               (static_cast<uint8_t>(dir) & 0b11))};
}
[[nodiscard]] constexpr StraightLine
decode_straight_line(const uint8_t *data) noexcept {
  return StraightLine{.xs = data[0],
                      .ys = data[1],
                      .len = static_cast<grid_t>((data[2] >> 3) & 0b11111),
                      .dir = static_cast<Direction>(data[2] & 0b11)};
}

namespace constexpr_tests {
constexpr StraightLine expectation{
    .xs = 14, .ys = 12, .len = 3, .dir = Direction::DOWN};
static_assert(expectation ==
              decode_straight_line(
                  std::data(encode_straight_line(14, 12, Direction::DOWN, 3))));
constexpr std::array<uint8_t, 3> straight_line_encoded_expectation{14U, 12U,
                                                                   0b00011010};
constexpr auto encoded_dut{encode_straight_line(14, 12, Direction::DOWN, 3)};
static_assert(std::equal(std::begin(encoded_dut), std::end(encoded_dut),
                         std::begin(straight_line_encoded_expectation)));

} // namespace constexpr_tests

[[nodiscard]] constexpr std::array<uint8_t, 3>
encode_rectangle(grid_t left, grid_t top, grid_t right,
                 grid_t bottom) noexcept {
  /*
   * byte[0][7:3] = left
   * byte[0][2:0]byte[1][7:6] = top
   * byte[1][4:0] = right
   * byte[2] = bottom
   */
  return {
      static_cast<uint8_t>(((left & 0b11111) << 3) | ((top & 0b11100) >> 2)),
      static_cast<uint8_t>(((top & 0b11) << 6) | (right & 0b11111)), bottom};
}

/* =======================================================================
                       _                   _
                      | |    _____   _____| |___
                      | |   / _ \ \ / / _ \ / __|
                      | |__|  __/\ V /  __/ \__ \
                      |_____\___| \_/ \___|_|___/

 * ======================================================================= */
static constexpr auto level_1_line_data{
    embp::concat(encode_straight_line(8, 8, Direction::DOWN, 33 - 2 - 7 - 7),
                 encode_straight_line(24, 8, Direction::DOWN, 33 - 2 - 7 - 7))};
inline constexpr std::array<Structure, 1> level_1_lines{
    Structure{.type = StructureType::STRAIGHT_LINE,
              .len = 2,
              .data = std::data(level_1_line_data)}};
inline constexpr Level level_1{.len = std::size(level_1_lines),
                               .data = std::data(level_1_lines)};

static constexpr auto level_2_line_data{embp::concat(
    encode_straight_line(8, 8, Direction::RIGHT, 33 - 2 - 7 - 7),
    encode_straight_line(16, 9, Direction::DOWN, 33 - 2 - 7 - 7 - 1))};
inline constexpr std::array<Structure, 1> level_2_lines{
    Structure{.type = StructureType::STRAIGHT_LINE,
              .len = 2,
              .data = std::data(level_2_line_data)}};
inline constexpr Level level_2{.len = std::size(level_2_lines),
                               .data = std::data(level_2_lines)};

static constexpr auto level_3_line_data{
    embp::concat(encode_straight_line(8, 8, Direction::RIGHT, 17),
                 encode_straight_line(8, 16, Direction::DOWN, 16),
                 encode_straight_line(24, 16, Direction::DOWN, 16))};
inline constexpr std::array<Structure, 1> level_3_lines{
    Structure{.type = StructureType::STRAIGHT_LINE,
              .len = 3,
              .data = std::data(level_3_line_data)}};
inline constexpr Level level_3{.len = std::size(level_3_lines),
                               .data = std::data(level_3_lines)};

/* Is a slight modification of level 1... I wonder if I'm going to notice a
 * pattern? */
static constexpr auto level_4_line_data{
    embp::concat(encode_straight_line(9, 8, Direction::RIGHT, 4),
                 encode_straight_line(9, 24, Direction::RIGHT, 4),
                 encode_straight_line(24 - 4, 8, Direction::RIGHT, 4),
                 encode_straight_line(24 - 4, 24, Direction::RIGHT, 4))};
inline constexpr std::array<Structure, 2> level_4_lines{
    level_1_lines[0], Structure{.type = StructureType::STRAIGHT_LINE,
                                .len = 4,
                                .data = std::data(level_4_line_data)}};
inline constexpr Level level_4{.len = std::size(level_4_lines),
                               .data = std::data(level_4_lines)};

/* Another slight modification of level 1... two makes a trend? */
static constexpr auto level_5_point_data{encode_point(16, 16)};
static constexpr auto level_5_line_data{
    embp::concat(encode_straight_line(9, 24, Direction::RIGHT, 8),
                 encode_straight_line(24 - 8, 8, Direction::RIGHT, 8))};
inline constexpr std::array<Structure, 3> level_5_lines{
    level_1_lines[0],
    Structure{.type = StructureType::STRAIGHT_LINE,
              .len = 2,
              .data = std::data(level_5_line_data)},
    Structure{.type = StructureType::POINT,
              .len = 1,
              .data = std::data(level_5_point_data)}};
inline constexpr Level level_5{.len = std::size(level_5_lines),
                               .data = std::data(level_5_lines)};

inline constexpr std::array levels{
    level_1, level_2, level_3, level_4, level_5,
    // level_6,
    // level_7,
    // level_8,
    // level_9,
    // level_10,
    // level_11,
    // level_12,
    // level_13,
    // level_14,
    // level_15,
    // level_16,
    // level_17,
    // level_18,
    // level_19,
    // level_20,
    // level_21,
    // level_22,
    // level_23,
    // level_24,
    // level_25,
    // level_26,
    // level_27,
    // level_28,
    // level_29,
    // level_30,
};
} // namespace snake

#endif