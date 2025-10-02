#if !defined(SNAKE_LEVEL_CONSTEXPR_HPP)
#define SNAKE_LEVEL_CONSTEXPR_HPP

#include <algorithm>
#include <array>

#include "common/utilities.hpp"
#include "snake_common.hpp"

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
  return lhs.x == rhs.x && lhs.y == rhs.y;
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

[[nodiscard]] constexpr bool check_intersects(GridLocation point,
                                              StraightLine linedef) noexcept {
  bool status{false};
  switch (linedef.dir) {
  case snake::Direction::DOWN:
    status = point.x == linedef.xs && point.y >= linedef.ys &&
             point.y < linedef.ys + linedef.len;
    break;
  case snake::Direction::RIGHT:
    status = point.y == linedef.ys && point.x >= linedef.xs &&
             point.x < linedef.xs + linedef.len;
    break;
  case snake::Direction::UP:
    status = point.x == linedef.xs && point.y >= linedef.ys - linedef.len + 1 &&
             point.y <= linedef.ys;
    break;
  case snake::Direction::LEFT:
    status = point.y == linedef.ys && point.x >= linedef.xs - linedef.len + 1 &&
             point.x <= linedef.xs;
    break;
  }
  return status;
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

struct Rectangle {
  grid_t left;
  grid_t top;
  grid_t right;  /* inclusive */
  grid_t bottom; /* inclusive */
};

[[nodiscard]] constexpr bool operator==(const Rectangle &lhs,
                                        const Rectangle &rhs) noexcept {
  return lhs.left == rhs.left && lhs.top == rhs.top && lhs.right == rhs.right &&
         lhs.bottom == rhs.bottom;
}

[[nodiscard]] constexpr bool check_intersects(GridLocation point,
                                              Rectangle rect) noexcept {
  return point.x <= rect.right && point.x >= rect.left && point.y >= rect.top &&
         point.y <= rect.bottom;
}

/** @brief encode a rectangle
 *
 * @param left column, or x, position of the left edge
 * @param top row, or y, position of the top edge
 * @param right column, or x, position of the right edge
 * @param bottom row, or y, position of the bottom edge
 *
 * @return Encoded byte array representation of the rectangle.
 */
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

[[nodiscard]] constexpr Rectangle
decode_rectangle(const uint8_t *data) noexcept {
  /*
   * byte[0][7:3] = left
   * byte[0][2:0]byte[1][7:6] = top
   * byte[1][4:0] = right
   * byte[2] = bottom
   */
  return Rectangle{.left = static_cast<grid_t>((data[0] >> 3U) & 0b11111U),
                   .top = static_cast<grid_t>(((data[0] & 0b111) << 2) |
                                              ((data[1] >> 6) & 0b11)),
                   .right = static_cast<grid_t>(data[1] & 0b11111),
                   .bottom = data[2]};
}

namespace constexpr_tests {
constexpr Rectangle rect_dut{.left = 10, .top = 11, .right = 20, .bottom = 21};
constexpr std::array<uint8_t, 3> expected_encoded_rect{
    (10 << 3) | ((11 & 0b11100) >> 2), ((11 & 0b11) << 6) | 20, 21};
constexpr auto encoded_rect_dut{encode_rectangle(
    rect_dut.left, rect_dut.top, rect_dut.right, rect_dut.bottom)};

static_assert(std::equal(std::begin(encoded_rect_dut),
                         std::end(encoded_rect_dut),
                         std::begin(expected_encoded_rect),
                         std::end(expected_encoded_rect)));

static_assert(rect_dut == decode_rectangle(std::data(encoded_rect_dut)));

[[nodiscard]] constexpr bool test_check_intersects_rect() noexcept {

  bool result{true};

  const Rectangle rect_dut{.left = 10, .top = 11, .right = 11, .bottom = 13};

  auto &&xpos_constant_test{[rect_dut](const grid_t xpos) {
    for (uint8_t ypos = rect_dut.top; ypos <= rect_dut.bottom; ++ypos) {
      return check_intersects({.x = xpos, .y = ypos}, rect_dut);
    }
  }};
  auto &&ypos_constant_test{[rect_dut](const grid_t ypos) {
    for (uint8_t xpos = rect_dut.left; xpos <= rect_dut.right; ++xpos) {
      return check_intersects({.x = xpos, .y = ypos}, rect_dut);
    }
  }};

  /* these tests should always return FALSE */
  /* check along just out the left edge */
  result &= !xpos_constant_test(static_cast<grid_t>(rect_dut.left - 1));
  /* repeat, but for the right side */
  result &= !xpos_constant_test(static_cast<grid_t>(rect_dut.right + 1));
  /* switch axis; check just above the top edge */
  result &= !ypos_constant_test(static_cast<grid_t>(rect_dut.top - 1));
  /* and again, but for the bottom edge */
  result &= !ypos_constant_test(static_cast<grid_t>(rect_dut.bottom + 1));

  /* these tests should always return TRUE */
  for (grid_t xpos = rect_dut.left; xpos <= rect_dut.right; ++xpos) {
    result &= xpos_constant_test(static_cast<grid_t>(xpos));
  }
  for (grid_t ypos = rect_dut.top; ypos <= rect_dut.bottom; ++ypos) {
    result &= ypos_constant_test(static_cast<grid_t>(ypos));
  }

  return result;
}
static_assert(test_check_intersects_rect());
} // namespace constexpr_tests

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
inline constexpr std::array<Structure, 1> level_1_structures{
    Structure{.type = StructureType::STRAIGHT_LINE,
              .len = 2,
              .data = std::data(level_1_line_data)}};
inline constexpr Level level_1{.len = std::size(level_1_structures),
                               .data = std::data(level_1_structures)};

static constexpr auto level_2_line_data{embp::concat(
    encode_straight_line(8, 8, Direction::RIGHT, 33 - 2 - 7 - 7),
    encode_straight_line(16, 9, Direction::DOWN, 33 - 2 - 7 - 7 - 1))};
inline constexpr std::array<Structure, 1> level_2_structures{
    Structure{.type = StructureType::STRAIGHT_LINE,
              .len = 2,
              .data = std::data(level_2_line_data)}};
inline constexpr Level level_2{.len = std::size(level_2_structures),
                               .data = std::data(level_2_structures)};

static constexpr auto level_3_line_data{
    embp::concat(encode_straight_line(8, 8, Direction::RIGHT, 17),
                 encode_straight_line(8, 16, Direction::DOWN, 16),
                 encode_straight_line(24, 16, Direction::DOWN, 16))};
inline constexpr std::array<Structure, 1> level_3_structures{
    Structure{.type = StructureType::STRAIGHT_LINE,
              .len = 3,
              .data = std::data(level_3_line_data)}};
inline constexpr Level level_3{.len = std::size(level_3_structures),
                               .data = std::data(level_3_structures)};

/* Is a slight modification of level 1... I wonder if I'm going to notice a
 * pattern? */
static constexpr auto level_4_line_data{
    embp::concat(encode_straight_line(9, 8, Direction::RIGHT, 4),
                 encode_straight_line(9, 24, Direction::RIGHT, 4),
                 encode_straight_line(24 - 4, 8, Direction::RIGHT, 4),
                 encode_straight_line(24 - 4, 24, Direction::RIGHT, 4))};
inline constexpr std::array<Structure, 2> level_4_structures{
    level_1_structures[0], Structure{.type = StructureType::STRAIGHT_LINE,
                                     .len = 4,
                                     .data = std::data(level_4_line_data)}};
inline constexpr Level level_4{.len = std::size(level_4_structures),
                               .data = std::data(level_4_structures)};

/* Another slight modification of level 1... two makes a trend? */
static constexpr auto level_5_point_data{encode_point(16, 16)};
static constexpr auto level_5_line_data{
    embp::concat(encode_straight_line(9, 24, Direction::RIGHT, 8),
                 encode_straight_line(24 - 8, 8, Direction::RIGHT, 8))};
inline constexpr std::array<Structure, 3> level_5_structures{
    level_1_structures[0],
    Structure{.type = StructureType::STRAIGHT_LINE,
              .len = 2,
              .data = std::data(level_5_line_data)},
    Structure{.type = StructureType::POINT,
              .len = 1,
              .data = std::data(level_5_point_data)}};
inline constexpr Level level_5{.len = std::size(level_5_structures),
                               .data = std::data(level_5_structures)};

static constexpr auto level_6_line_data{
    embp::concat(encode_straight_line(6, 5, Direction::DOWN, 23),
                 encode_straight_line(9, 5, Direction::DOWN, 23),
                 encode_straight_line(23, 5, Direction::DOWN, 23),
                 encode_straight_line(26, 5, Direction::DOWN, 23))};
inline constexpr std::array level_6_structures{
    Structure{.type = StructureType::STRAIGHT_LINE,
              .len = 4,
              .data = std::data(level_6_line_data)}};
inline constexpr Level level_6{.len = std::size(level_6_structures),
                               .data = std::data(level_6_structures)};

static constexpr auto level_7_line_data{
    embp::concat(encode_straight_line(5, 5, Direction::RIGHT, 23),
                 encode_straight_line(5, 8, Direction::RIGHT, 10),
                 encode_straight_line(18, 8, Direction::RIGHT, 10),
                 encode_straight_line(14, 9, Direction::DOWN, 19),
                 encode_straight_line(18, 9, Direction::DOWN, 19))};
inline constexpr std::array level_7_structures{
    Structure{.type = StructureType::STRAIGHT_LINE,
              .len = 5,
              .data = std::data(level_7_line_data)}};
inline constexpr Level level_7{.len = std::size(level_7_structures),
                               .data = std::data(level_7_structures)};

static constexpr auto level_8_line_data{
    embp::concat(encode_straight_line(5, 5, Direction::RIGHT, 23),
                 encode_straight_line(5, 8, Direction::RIGHT, 23),
                 encode_straight_line(5, 12, Direction::DOWN, 20),
                 encode_straight_line(8, 12, Direction::DOWN, 20),
                 encode_straight_line(24, 12, Direction::DOWN, 20),
                 encode_straight_line(27, 12, Direction::DOWN, 20))};
inline constexpr std::array level_8_structures{
    Structure{.type = StructureType::STRAIGHT_LINE,
              .len = 6,
              .data = std::data(level_8_line_data)}};
inline constexpr Level level_8{.len = std::size(level_8_structures),
                               .data = std::data(level_8_structures)};

static constexpr auto level_9_point_data{
    embp::concat(encode_point(13, 8), encode_point(19, 8), encode_point(13, 24),
                 encode_point(19, 24))};
static constexpr auto level_9_line_data{
    embp::concat(encode_straight_line(5, 5, Direction::RIGHT, 9),
                 encode_straight_line(19, 5, Direction::RIGHT, 9),
                 encode_straight_line(5, 27, Direction::RIGHT, 9),
                 encode_straight_line(19, 27, Direction::RIGHT, 9),
                 encode_straight_line(5, 6, Direction::DOWN, 21),
                 encode_straight_line(27, 6, Direction::DOWN, 21))};
inline constexpr std::array level_9_structures{
    level_4_structures[0], level_4_structures[1],
    Structure{.type = StructureType::STRAIGHT_LINE,
              .len = 6,
              .data = std::data(level_9_line_data)},
    Structure{.type = StructureType::POINT,
              .len = 4,
              .data = std::data(level_9_point_data)}};
inline constexpr Level level_9{.len = std::size(level_9_structures),
                               .data = std::data(level_9_structures)};

static constexpr auto level_10_point_data{
    embp::concat(encode_point(15, 15), encode_point(15, 17),
                 encode_point(17, 15), encode_point(17, 17))};
static constexpr auto level_10_line_data{
    embp::concat(encode_straight_line(5, 5, Direction::DOWN, 23),
                 encode_straight_line(6, 27, Direction::RIGHT, 13),
                 encode_straight_line(8, 5, Direction::DOWN, 20),
                 encode_straight_line(9, 24, Direction::RIGHT, 10),
                 encode_straight_line(14, 5, Direction::RIGHT, 13),
                 encode_straight_line(27, 5, Direction::DOWN, 23),
                 encode_straight_line(14, 8, Direction::RIGHT, 10),
                 encode_straight_line(24, 8, Direction::DOWN, 20))};
inline constexpr std::array level_10_structures{
    Structure{.type = StructureType::STRAIGHT_LINE,
              .len = 8,
              .data = std::data(level_10_line_data)},
    Structure{.type = StructureType::POINT,
              .len = 4,
              .data = std::data(level_10_point_data)}};
inline constexpr Level level_10{.len = std::size(level_10_structures),
                                .data = std::data(level_10_structures)};

static constexpr auto level_11_line_data{
    embp::concat(encode_straight_line(6, 5, Direction::DOWN, 23),
                 encode_straight_line(9, 5, Direction::DOWN, 23),
                 encode_straight_line(12, 5, Direction::DOWN, 23),
                 encode_straight_line(20, 5, Direction::DOWN, 23),
                 encode_straight_line(23, 5, Direction::DOWN, 23),
                 encode_straight_line(26, 5, Direction::DOWN, 23))};
inline constexpr std::array level_11_structures{
    Structure{.type = StructureType::STRAIGHT_LINE,
              .len = 6,
              .data = std::data(level_11_line_data)}};
inline constexpr Level level_11{.len = std::size(level_11_structures),
                                .data = std::data(level_11_structures)};

static constexpr auto level_12_line_data{
    embp::concat(encode_straight_line(4, 4, Direction::RIGHT, 25),
                 encode_straight_line(4, 8, Direction::RIGHT, 10),
                 encode_straight_line(4, 12, Direction::RIGHT, 6),
                 encode_straight_line(19, 8, Direction::RIGHT, 10),
                 encode_straight_line(22, 12, Direction::RIGHT, 6),
                 encode_straight_line(14, 8, Direction::DOWN, 21),
                 encode_straight_line(18, 8, Direction::DOWN, 21),
                 encode_straight_line(10, 12, Direction::DOWN, 17),
                 encode_straight_line(22, 12, Direction::DOWN, 17))};
inline constexpr std::array level_12_structures{
    Structure{.type = StructureType::STRAIGHT_LINE,
              .len = 9,
              .data = std::data(level_12_line_data)}};
inline constexpr Level level_12{.len = std::size(level_12_structures),
                                .data = std::data(level_12_structures)};

static constexpr auto level_13_point_data{
    embp::concat(encode_point(6, 30), encode_point(26, 30))};
static constexpr auto level_13_line_data{
    embp::concat(encode_straight_line(4, 4, Direction::RIGHT, 25),
                 encode_straight_line(4, 8, Direction::RIGHT, 25),
                 encode_straight_line(4, 12, Direction::DOWN, 20),
                 encode_straight_line(8, 12, Direction::DOWN, 20),
                 encode_straight_line(24, 12, Direction::DOWN, 20),
                 encode_straight_line(28, 12, Direction::DOWN, 20),
                 encode_straight_line(6, 15, Direction::DOWN, 14),
                 encode_straight_line(26, 15, Direction::DOWN, 14))};
inline constexpr std::array level_13_structures{
    Structure{.type = StructureType::POINT,
              .len = 2,
              .data = std::data(level_13_point_data)},
    Structure{.type = StructureType::STRAIGHT_LINE,
              .len = 8,
              .data = std::data(level_13_line_data)}};
inline constexpr Level level_13{.len = std::size(level_13_structures),
                                .data = std::data(level_13_structures)};

static constexpr auto level_14_line_data{
    embp::concat(encode_straight_line(4, 4, Direction::DOWN, 25),
                 encode_straight_line(28, 4, Direction::DOWN, 25),
                 encode_straight_line(8, 8, Direction::DOWN, 17),
                 encode_straight_line(24, 8, Direction::DOWN, 17),
                 encode_straight_line(12, 12, Direction::DOWN, 3),
                 encode_straight_line(12, 18, Direction::DOWN, 3),
                 encode_straight_line(20, 12, Direction::DOWN, 3),
                 encode_straight_line(20, 18, Direction::DOWN, 3),
                 encode_straight_line(5, 4, Direction::RIGHT, 10),
                 encode_straight_line(18, 4, Direction::RIGHT, 10),
                 encode_straight_line(5, 28, Direction::RIGHT, 10),
                 encode_straight_line(18, 28, Direction::RIGHT, 10),
                 encode_straight_line(9, 8, Direction::RIGHT, 6),
                 encode_straight_line(18, 8, Direction::RIGHT, 6),
                 encode_straight_line(9, 24, Direction::RIGHT, 6),
                 encode_straight_line(18, 24, Direction::RIGHT, 6),
                 encode_straight_line(13, 12, Direction::RIGHT, 7),
                 encode_straight_line(13, 20, Direction::RIGHT, 7))};

inline constexpr std::array level_14_structures{
    Structure{.type = StructureType::STRAIGHT_LINE,
              .len = 18,
              .data = std::data(level_14_line_data)}};
inline constexpr Level level_14{.len = std::size(level_14_structures),
                                .data = std::data(level_14_structures)};

static constexpr auto level_15_point_data{embp::concat(encode_point(16, 16))};
static constexpr auto level_15_line_data{
    embp::concat(encode_straight_line(4, 4, Direction::DOWN, 25),
                 encode_straight_line(28, 4, Direction::DOWN, 25),
                 encode_straight_line(8, 4, Direction::DOWN, 21),
                 encode_straight_line(24, 8, Direction::DOWN, 21),
                 encode_straight_line(5, 28, Direction::RIGHT, 16),
                 encode_straight_line(12, 4, Direction::RIGHT, 16),
                 encode_straight_line(9, 24, Direction::RIGHT, 12),
                 encode_straight_line(12, 8, Direction::RIGHT, 12),
                 encode_straight_line(13, 13, Direction::RIGHT, 3),
                 encode_straight_line(13, 14, Direction::RIGHT, 3),
                 encode_straight_line(13, 15, Direction::RIGHT, 2),
                 encode_straight_line(13, 17, Direction::RIGHT, 2),
                 encode_straight_line(13, 18, Direction::RIGHT, 3),
                 encode_straight_line(13, 19, Direction::RIGHT, 3),
                 encode_straight_line(17, 13, Direction::RIGHT, 3),
                 encode_straight_line(17, 14, Direction::RIGHT, 3),
                 encode_straight_line(18, 15, Direction::RIGHT, 2),
                 encode_straight_line(18, 17, Direction::RIGHT, 2),
                 encode_straight_line(17, 18, Direction::RIGHT, 3),
                 encode_straight_line(17, 19, Direction::RIGHT, 3))};

inline constexpr std::array level_15_structures{
    Structure{.type = StructureType::POINT,
              .len = 1,
              .data = std::data(level_15_point_data)},
    Structure{.type = StructureType::STRAIGHT_LINE,
              .len = 20,
              .data = std::data(level_15_line_data)}};
inline constexpr Level level_15{.len = std::size(level_15_structures),
                                .data = std::data(level_15_structures)};

static constexpr auto level_16_line_data{
    embp::concat(encode_straight_line(3, 5, Direction::DOWN, 23),
                 encode_straight_line(6, 5, Direction::DOWN, 23),
                 encode_straight_line(9, 5, Direction::DOWN, 23),
                 encode_straight_line(12, 5, Direction::DOWN, 23),
                 encode_straight_line(20, 5, Direction::DOWN, 23),
                 encode_straight_line(23, 5, Direction::DOWN, 23),
                 encode_straight_line(26, 5, Direction::DOWN, 23),
                 encode_straight_line(29, 5, Direction::DOWN, 23))};

inline constexpr std::array level_16_structures{
    Structure{.type = StructureType::STRAIGHT_LINE,
              .len = 8,
              .data = std::data(level_16_line_data)}};
inline constexpr Level level_16{.len = std::size(level_16_structures),
                                .data = std::data(level_16_structures)};

static constexpr auto level_17_line_data{
    embp::concat(encode_straight_line(4, 4, Direction::RIGHT, 25),
                 encode_straight_line(4, 8, Direction::RIGHT, 11),
                 encode_straight_line(18, 8, Direction::RIGHT, 11),
                 encode_straight_line(4, 12, Direction::RIGHT, 7),
                 encode_straight_line(22, 12, Direction::RIGHT, 7),
                 encode_straight_line(4, 16, Direction::RIGHT, 3),
                 encode_straight_line(26, 16, Direction::RIGHT, 3),
                 encode_straight_line(6, 17, Direction::DOWN, 12),
                 encode_straight_line(26, 17, Direction::DOWN, 12),
                 encode_straight_line(10, 13, Direction::DOWN, 16),
                 encode_straight_line(22, 13, Direction::DOWN, 16),
                 encode_straight_line(14, 9, Direction::DOWN, 22),
                 encode_straight_line(18, 9, Direction::DOWN, 22))};
inline constexpr std::array level_17_structures{
    Structure{.type = StructureType::STRAIGHT_LINE,
              .len = 13,
              .data = std::data(level_17_line_data)}};
inline constexpr Level level_17{.len = std::size(level_17_structures),
                                .data = std::data(level_17_structures)};

static constexpr auto level_18_point_data{
    embp::concat(encode_point(2, 30), encode_point(30, 30))};
static constexpr auto level_18_line_data{
    embp::concat(encode_straight_line(2, 15, Direction::DOWN, 14),
                 encode_straight_line(30, 15, Direction::DOWN, 14),
                 encode_straight_line(12, 12, Direction::RIGHT, 9),
                 encode_straight_line(12, 16, Direction::RIGHT, 9),
                 encode_straight_line(12, 20, Direction::RIGHT, 9),
                 encode_straight_line(12, 24, Direction::RIGHT, 9))};
inline constexpr std::array level_18_structures{
    level_13_structures[0], level_13_structures[1],
    Structure{.type = StructureType::POINT,
              .len = 2,
              .data = std::data(level_18_point_data)},
    Structure{.type = StructureType::STRAIGHT_LINE,
              .len = 6,
              .data = std::data(level_18_line_data)}};
inline constexpr Level level_18{.len = std::size(level_18_structures),
                                .data = std::data(level_18_structures)};

static constexpr auto level_19_point_data{
    embp::concat(encode_point(16, 14), encode_point(16, 18),
                 encode_point(14, 16), encode_point(18, 16))};
static constexpr auto level_19_line_data{
    embp::concat(encode_straight_line(4, 4, Direction::DOWN, 25),
                 encode_straight_line(28, 4, Direction::DOWN, 25),
                 encode_straight_line(5, 4, Direction::RIGHT, 10),
                 encode_straight_line(18, 4, Direction::RIGHT, 10),
                 encode_straight_line(5, 28, Direction::RIGHT, 10),
                 encode_straight_line(18, 28, Direction::RIGHT, 10),
                 encode_straight_line(8, 8, Direction::RIGHT, 17),
                 encode_straight_line(8, 24, Direction::RIGHT, 17),
                 encode_straight_line(8, 9, Direction::DOWN, 6),
                 encode_straight_line(8, 18, Direction::DOWN, 6),
                 encode_straight_line(24, 9, Direction::DOWN, 6),
                 encode_straight_line(24, 18, Direction::DOWN, 6),
                 encode_straight_line(12, 12, Direction::RIGHT, 3),
                 encode_straight_line(18, 12, Direction::RIGHT, 3),
                 encode_straight_line(12, 20, Direction::RIGHT, 3),
                 encode_straight_line(18, 20, Direction::RIGHT, 3),
                 encode_straight_line(12, 13, Direction::DOWN, 7),
                 encode_straight_line(20, 13, Direction::DOWN, 7))};
inline constexpr std::array level_19_structures{
    Structure{.type = StructureType::POINT,
              .len = 4,
              .data = std::data(level_19_point_data)},
    Structure{.type = StructureType::STRAIGHT_LINE,
              .len = 18,
              .data = std::data(level_19_line_data)}};
inline constexpr Level level_19{.len = std::size(level_19_structures),
                                .data = std::data(level_19_structures)};

static constexpr auto level_20_point_data{embp::concat(encode_point(16, 16))};
static constexpr auto level_20_line_data{
    embp::concat(encode_straight_line(4, 4, Direction::DOWN, 25),
                 encode_straight_line(28, 4, Direction::DOWN, 25),
                 encode_straight_line(8, 2, Direction::DOWN, 23),
                 encode_straight_line(24, 8, Direction::DOWN, 23),
                 encode_straight_line(12, 4, Direction::RIGHT, 16),
                 encode_straight_line(5, 28, Direction::RIGHT, 16),
                 encode_straight_line(12, 8, Direction::RIGHT, 12),
                 encode_straight_line(9, 24, Direction::RIGHT, 12),
                 encode_straight_line(13, 13, Direction::RIGHT, 3),
                 encode_straight_line(13, 15, Direction::RIGHT, 2),
                 encode_straight_line(13, 17, Direction::DOWN, 3),
                 encode_straight_line(17, 13, Direction::DOWN, 2),
                 encode_straight_line(19, 13, Direction::DOWN, 3),
                 encode_straight_line(15, 18, Direction::DOWN, 2),
                 encode_straight_line(17, 19, Direction::RIGHT, 3),
                 encode_straight_line(18, 17, Direction::RIGHT, 2))};
inline constexpr std::array level_20_structures{
    Structure{.type = StructureType::POINT,
              .len = 1,
              .data = std::data(level_20_point_data)},
    Structure{.type = StructureType::STRAIGHT_LINE,
              .len = 16,
              .data = std::data(level_20_line_data)}};
inline constexpr Level level_20{.len = std::size(level_20_structures),
                                .data = std::data(level_20_structures)};

static constexpr auto level_21_line_data{
    embp::concat(encode_straight_line(4, 4, Direction::DOWN, 25),
                 encode_straight_line(8, 4, Direction::DOWN, 25),
                 encode_straight_line(12, 4, Direction::DOWN, 25),
                 encode_straight_line(16, 4, Direction::DOWN, 25),
                 encode_straight_line(20, 4, Direction::DOWN, 25),
                 encode_straight_line(24, 4, Direction::DOWN, 25),
                 encode_straight_line(28, 4, Direction::DOWN, 25))};
inline constexpr std::array level_21_structures{
    Structure{.type = StructureType::STRAIGHT_LINE,
              .len = 7,
              .data = std::data(level_21_line_data)}};
inline constexpr Level level_21{.len = std::size(level_21_structures),
                                .data = std::data(level_21_structures)};

static constexpr auto level_22_line_data{
    embp::concat(encode_straight_line(4, 4, Direction::RIGHT, 25),
                 encode_straight_line(1, 8, Direction::RIGHT, 14),
                 encode_straight_line(18, 8, Direction::RIGHT, 14),
                 encode_straight_line(14, 9, Direction::DOWN, 21),
                 encode_straight_line(18, 9, Direction::DOWN, 21),
                 encode_straight_line(4, 12, Direction::RIGHT, 7),
                 encode_straight_line(22, 12, Direction::RIGHT, 7),
                 encode_straight_line(10, 13, Direction::DOWN, 16),
                 encode_straight_line(22, 13, Direction::DOWN, 16),
                 encode_straight_line(3, 16, Direction::RIGHT, 4),
                 encode_straight_line(26, 16, Direction::RIGHT, 4),
                 encode_straight_line(6, 17, Direction::DOWN, 15),
                 encode_straight_line(26, 17, Direction::DOWN, 15))};
inline constexpr std::array level_22_structures{
    Structure{.type = StructureType::STRAIGHT_LINE,
              .len = 13,
              .data = std::data(level_22_line_data)}};
inline constexpr Level level_22{.len = std::size(level_22_structures),
                                .data = std::data(level_22_structures)};

static constexpr auto level_23_point_data{
    embp::concat(encode_point(16, 10), encode_point(16, 14),
                 encode_point(16, 18), encode_point(16, 22))};
static constexpr auto level_23_line_data{
    embp::concat(encode_straight_line(4, 4, Direction::RIGHT, 25),
                 encode_straight_line(7, 6, Direction::RIGHT, 19),
                 encode_straight_line(4, 8, Direction::RIGHT, 25),
                 encode_straight_line(4, 12, Direction::DOWN, 20),
                 encode_straight_line(8, 12, Direction::DOWN, 20),
                 encode_straight_line(12, 12, Direction::DOWN, 20),
                 encode_straight_line(20, 12, Direction::DOWN, 20),
                 encode_straight_line(24, 12, Direction::DOWN, 20),
                 encode_straight_line(28, 12, Direction::DOWN, 20),
                 encode_straight_line(2, 16, Direction::DOWN, 6),
                 encode_straight_line(2, 23, Direction::DOWN, 6),
                 encode_straight_line(6, 16, Direction::DOWN, 6),
                 encode_straight_line(6, 23, Direction::DOWN, 6),
                 encode_straight_line(10, 16, Direction::DOWN, 6),
                 encode_straight_line(10, 23, Direction::DOWN, 6),
                 encode_straight_line(22, 16, Direction::DOWN, 6),
                 encode_straight_line(22, 23, Direction::DOWN, 6),
                 encode_straight_line(26, 16, Direction::DOWN, 6),
                 encode_straight_line(26, 23, Direction::DOWN, 6),
                 encode_straight_line(30, 16, Direction::DOWN, 6),
                 encode_straight_line(30, 23, Direction::DOWN, 6),
                 encode_straight_line(14, 12, Direction::RIGHT, 5),
                 encode_straight_line(14, 16, Direction::RIGHT, 5),
                 encode_straight_line(14, 20, Direction::RIGHT, 5),
                 encode_straight_line(14, 24, Direction::RIGHT, 5))};
inline constexpr std::array level_23_structures{
    Structure{.type = StructureType::POINT,
              .len = 4,
              .data = std::data(level_23_point_data)},
    Structure{.type = StructureType::STRAIGHT_LINE,
              .len = 25,
              .data = std::data(level_23_line_data)}};
inline constexpr Level level_23{.len = std::size(level_23_structures),
                                .data = std::data(level_23_structures)};

static constexpr auto level_24_point_data{
    embp::concat(encode_point(15, 15), encode_point(15, 17),
                 encode_point(17, 15), encode_point(17, 17))};
static constexpr auto level_24_line_data{
    embp::concat(encode_straight_line(6, 4, Direction::RIGHT, 21),
                 encode_straight_line(10, 8, Direction::RIGHT, 13),
                 encode_straight_line(10, 24, Direction::RIGHT, 13),
                 encode_straight_line(4, 6, Direction::DOWN, 21),
                 encode_straight_line(28, 6, Direction::DOWN, 21),
                 encode_straight_line(14, 12, Direction::RIGHT, 5),
                 encode_straight_line(14, 20, Direction::RIGHT, 5),
                 encode_straight_line(12, 14, Direction::DOWN, 5),
                 encode_straight_line(20, 14, Direction::DOWN, 5),
                 encode_straight_line(6, 28, Direction::RIGHT, 10),
                 encode_straight_line(17, 28, Direction::RIGHT, 10),
                 encode_straight_line(8, 10, Direction::DOWN, 13),
                 encode_straight_line(24, 10, Direction::DOWN, 13))};
inline constexpr std::array level_24_structures{
    Structure{.type = StructureType::POINT,
              .len = 4,
              .data = std::data(level_24_point_data)},
    Structure{.type = StructureType::STRAIGHT_LINE,
              .len = 13,
              .data = std::data(level_24_line_data)}};
inline constexpr Level level_24{.len = std::size(level_24_structures),
                                .data = std::data(level_24_structures)};

static constexpr auto level_25_point_data{embp::concat(
    encode_point(16, 16), encode_point(13, 13), encode_point(19, 19),
    encode_point(13, 19), encode_point(19, 13))};
static constexpr auto level_25_line_data{
    embp::concat(encode_straight_line(4, 4, Direction::DOWN, 25),
                 encode_straight_line(28, 4, Direction::DOWN, 25),
                 encode_straight_line(7, 2, Direction::DOWN, 24),
                 encode_straight_line(25, 7, Direction::DOWN, 24),
                 encode_straight_line(9, 4, Direction::RIGHT, 19),
                 encode_straight_line(5, 28, Direction::RIGHT, 19),
                 encode_straight_line(11, 7, Direction::RIGHT, 14),
                 encode_straight_line(8, 25, Direction::RIGHT, 14),
                 encode_straight_line(15, 9, Direction::RIGHT, 3),
                 encode_straight_line(15, 11, Direction::RIGHT, 3),
                 encode_straight_line(15, 21, Direction::RIGHT, 3),
                 encode_straight_line(15, 23, Direction::RIGHT, 3),
                 encode_straight_line(9, 15, Direction::DOWN, 3),
                 encode_straight_line(11, 15, Direction::DOWN, 3),
                 encode_straight_line(21, 15, Direction::DOWN, 3),
                 encode_straight_line(23, 15, Direction::DOWN, 3),
                 encode_straight_line(15, 13, Direction::DOWN, 2),
                 encode_straight_line(17, 13, Direction::DOWN, 2),
                 encode_straight_line(15, 18, Direction::DOWN, 2),
                 encode_straight_line(17, 18, Direction::DOWN, 2),
                 encode_straight_line(13, 15, Direction::RIGHT, 2),
                 encode_straight_line(13, 17, Direction::RIGHT, 2),
                 encode_straight_line(18, 15, Direction::RIGHT, 2),
                 encode_straight_line(18, 17, Direction::RIGHT, 2))};
inline constexpr std::array level_25_structures{
    Structure{.type = StructureType::POINT,
              .len = 5,
              .data = std::data(level_25_point_data)},
    Structure{.type = StructureType::STRAIGHT_LINE,
              .len = 24,
              .data = std::data(level_25_line_data)}};
inline constexpr Level level_25{.len = std::size(level_25_structures),
                                .data = std::data(level_25_structures)};

static constexpr auto level_26_point_data{
    embp::concat(encode_point(12, 14), encode_point(14, 14),
                 encode_point(18, 18), encode_point(20, 18))};
static constexpr auto level_26_line_data{
    embp::concat(encode_straight_line(7, 8, Direction::DOWN, 17),
                 encode_straight_line(10, 11, Direction::DOWN, 14),
                 encode_straight_line(16, 11, Direction::DOWN, 11),
                 encode_straight_line(22, 8, Direction::DOWN, 14),
                 encode_straight_line(25, 8, Direction::DOWN, 17),
                 encode_straight_line(8, 8, Direction::RIGHT, 14),
                 encode_straight_line(11, 11, Direction::RIGHT, 5),
                 encode_straight_line(17, 21, Direction::RIGHT, 5),
                 encode_straight_line(11, 24, Direction::RIGHT, 14))};
inline constexpr std::array level_26_structures{
    Structure{.type = StructureType::POINT,
              .len = 4,
              .data = std::data(level_26_point_data)},
    Structure{.type = StructureType::STRAIGHT_LINE,
              .len = 9,
              .data = std::data(level_26_line_data)}};
inline constexpr Level level_26{.len = std::size(level_26_structures),
                                .data = std::data(level_26_structures)};

/* clang-format off */
static constexpr auto level_27_point_data{
    embp::concat(
            /* top left */
            encode_point(3,3), 
            encode_point(5,5),
            encode_point(15,5),
            encode_point(5,15),
            encode_point(15,13),
            /* bot right*/
            encode_point(32-3,32-3), 
            encode_point(32-5,32-5),
            encode_point(32-15,32-5),
            encode_point(32-5,32-15),
            encode_point(32-15,32-13),
            /* top right*/
            encode_point(32-3,3), 
            encode_point(32-5,5),
            encode_point(32-15,5),
            encode_point(32-5,15),
            encode_point(32-15,13),
            /* bot left */
            encode_point(3,32-3), 
            encode_point(5,32-5),
            encode_point(15,32-5),
            encode_point(5,32-15),
            encode_point(15,32-13),
            /* center */
            encode_point(16,16)
                )};
static constexpr auto level_27_line_data{
    embp::concat(
            /* top left */
            encode_straight_line(5, 4, Direction::RIGHT, 11),
            encode_straight_line(4, 5, Direction::DOWN, 11),
            encode_straight_line(6, 11, Direction::DOWN, 2),
            encode_straight_line(11, 6, Direction::DOWN, 2),
            encode_straight_line(7, 15, Direction::RIGHT, 4),
            encode_straight_line(15, 7, Direction::DOWN, 4),
            encode_straight_line(11, 11, Direction::RIGHT, 2),
            encode_straight_line(11, 12, Direction::RIGHT, 2),
            encode_straight_line(13, 15, Direction::RIGHT, 2),
            encode_straight_line(14, 14, Direction::RIGHT, 2),
            /* bot right*/
            encode_straight_line(32-5, 32-4, Direction::LEFT, 11),
            encode_straight_line(32-4, 32-5, Direction::UP, 11),
            encode_straight_line(32-6, 32-11, Direction::UP, 2),
            encode_straight_line(32-11, 32-6, Direction::UP, 2),
            encode_straight_line(32-7, 32-15, Direction::LEFT, 4),
            encode_straight_line(32-15, 32-7, Direction::UP, 4),
            encode_straight_line(32-11, 32-11, Direction::LEFT, 2),
            encode_straight_line(32-11, 32-12, Direction::LEFT, 2),
            encode_straight_line(32-13, 32-15, Direction::LEFT, 2),
            encode_straight_line(32-14, 32-14, Direction::LEFT, 2),
            /* top right*/
            encode_straight_line(32-5, 4, Direction::LEFT, 11),
            encode_straight_line(32-4, 5, Direction::DOWN, 11),
            encode_straight_line(32-6, 11, Direction::DOWN, 2),
            encode_straight_line(32-11, 6, Direction::DOWN, 2),
            encode_straight_line(32-7, 15, Direction::LEFT, 4),
            encode_straight_line(32-15, 7, Direction::DOWN, 4),
            encode_straight_line(32-11, 11, Direction::LEFT, 2),
            encode_straight_line(32-11, 12, Direction::LEFT, 2),
            encode_straight_line(32-13, 15, Direction::LEFT, 2),
            encode_straight_line(32-14, 14, Direction::LEFT, 2),
            /* bot left */
            encode_straight_line(5, 32-4, Direction::RIGHT, 11),
            encode_straight_line(4, 32-5, Direction::UP, 11),
            encode_straight_line(6, 32-11, Direction::UP, 2),
            encode_straight_line(11, 32-6, Direction::UP, 2),
            encode_straight_line(7, 32-15, Direction::RIGHT, 4),
            encode_straight_line(15, 32-7, Direction::UP, 4),
            encode_straight_line(11, 32-11, Direction::RIGHT, 2),
            encode_straight_line(11, 32-12, Direction::RIGHT, 2),
            encode_straight_line(13, 32-15, Direction::RIGHT, 2),
            encode_straight_line(14, 32-14, Direction::RIGHT, 2)
                )};
/* clang-format on */
inline constexpr std::array level_27_structures{
    Structure{.type = StructureType::POINT,
              .len = 21,
              .data = std::data(level_27_point_data)},
    Structure{.type = StructureType::STRAIGHT_LINE,
              .len = 40,
              .data = std::data(level_27_line_data)}};
inline constexpr Level level_27{.len = std::size(level_27_structures),
                                .data = std::data(level_27_structures)};

/* clang-format off */
static constexpr auto level_28_point_data{
    embp::concat(
            /* top left */
            encode_point(2,2), 
            encode_point(7,13), 
            /* bot right*/
            encode_point(32-2,32-2), 
            encode_point(32-7,32-13), 
            /* top right*/
            encode_point(30, 2),
            encode_point(28, 4),
            encode_point(26, 6),
            encode_point(24, 7),
            encode_point(22, 9),
            encode_point(20,11),
            /* bot left */
            encode_point(32-30,32- 2),
            encode_point(32-28,32- 4),
            encode_point(32-26,32- 6),
            encode_point(32-24,32- 7),
            encode_point(32-22,32- 9),
            encode_point(32-20,32-11)
                )};
static constexpr auto level_28_line_data{
    embp::concat(
            /* top left */
encode_straight_line(14,1,Direction::DOWN,3),
encode_straight_line(14,5,Direction::DOWN,7),
encode_straight_line(1,16,Direction::RIGHT,2),
encode_straight_line(4,16,Direction::RIGHT,5),
encode_straight_line(13,11,Direction::DOWN,2),
encode_straight_line(12,12,Direction::DOWN,2),
encode_straight_line(11,13,Direction::DOWN,2),
encode_straight_line(10,14,Direction::DOWN,2),
encode_straight_line( 9,15,Direction::DOWN,2),
encode_straight_line(11, 9,Direction::DOWN,2),
encode_straight_line(10,10,Direction::DOWN,2),
encode_straight_line( 9,11,Direction::DOWN,2),
encode_straight_line( 8,12,Direction::DOWN,2),
            /* bot right*/
encode_straight_line(32-14,32-1,Direction::UP,3),
encode_straight_line(32-14,32-5,Direction::UP,7),
encode_straight_line(32-1,32-16,Direction::LEFT,2),
encode_straight_line(32-4,32-16,Direction::LEFT,5),
encode_straight_line(32-13,32-11,Direction::UP,2),
encode_straight_line(32-12,32-12,Direction::UP,2),
encode_straight_line(32-11,32-13,Direction::UP,2),
encode_straight_line(32-10,32-14,Direction::UP,2),
encode_straight_line(32- 9,32-15,Direction::UP,2),
encode_straight_line(32-11,32- 9,Direction::UP,2),
encode_straight_line(32-10,32-10,Direction::UP,2),
encode_straight_line(32- 9,32-11,Direction::UP,2),
encode_straight_line(32- 8,32-12,Direction::UP,2),
            /* top right*/
encode_straight_line(18,1,Direction::DOWN,3),
encode_straight_line(18,5,Direction::DOWN,9),
encode_straight_line(30,13,Direction::RIGHT,2),
encode_straight_line(19,13,Direction::RIGHT,10),
            /* bot left */
encode_straight_line(32-18,32-1,Direction::UP,3),
encode_straight_line(32-18,32-5,Direction::UP,9),
encode_straight_line(32-30,32-13,Direction::LEFT,2),
encode_straight_line(32-19,32-13,Direction::LEFT,10),
            /* center */
encode_straight_line(14,15,Direction::RIGHT,3),
encode_straight_line(13,16,Direction::RIGHT,7),
encode_straight_line(16,17,Direction::RIGHT,3)
                )};
/* clang-format on */
inline constexpr std::array level_28_structures{
    Structure{.type = StructureType::POINT,
              .len = 16,
              .data = std::data(level_28_point_data)},
    Structure{.type = StructureType::STRAIGHT_LINE,
              .len = 37,
              .data = std::data(level_28_line_data)}};
inline constexpr Level level_28{.len = std::size(level_28_structures),
                                .data = std::data(level_28_structures)};

/* clang-format off */
static constexpr auto level_29_point_data{
    embp::concat(
            /* top left */
            encode_point(2,2), 
            encode_point(6,6), 
            encode_point(15,5), 
            encode_point(5,15), 
            encode_point(12,13), 
            encode_point(14,14),
            /* bot right*/
            encode_point(32-2,32-2), 
            encode_point(32-6,32-6), 
            encode_point(32-15,32-5), 
            encode_point(32-5,32-15), 
            encode_point(32-12,32-13), 
            encode_point(32-14,32-14),
            /* top right*/
            encode_point(32-2,2), 
            encode_point(32-6,6), 
            encode_point(32-15,5), 
            encode_point(32-5,15), 
            encode_point(32-12,13), 
            encode_point(32-14,14),
            /* bot left */
            encode_point(2,32-2), 
            encode_point(6,32-6), 
            encode_point(15,32-5), 
            encode_point(5,32-15), 
            encode_point(12,32-13), 
            encode_point(14,32-14),
            /* other */
            encode_point(16,16),
            encode_point(15,30),
            encode_point(17,30)
                )};
static constexpr auto level_29_line_data{
    embp::concat(
            /* top left */
encode_straight_line(13,15,Direction::RIGHT,2),
encode_straight_line(15,13,Direction::DOWN,2),
encode_straight_line(12,12,Direction::RIGHT,2),
encode_straight_line(6,12,Direction::DOWN,2),
encode_straight_line(12,6,Direction::RIGHT,2),
encode_straight_line(4,4,Direction::RIGHT,12),
encode_straight_line(4,5,Direction::DOWN,11),
encode_straight_line(7,15,Direction::RIGHT,5),
encode_straight_line(15,7,Direction::DOWN,5),
            /* bot right*/
encode_straight_line(32-13,32-15,Direction::LEFT,2),
encode_straight_line(32-15,32-13,Direction::UP,2),
encode_straight_line(32-12,32-12,Direction::LEFT,2),
encode_straight_line(32-6,32-12,Direction::UP,2),
encode_straight_line(32-12,32-6,Direction::LEFT,2),
encode_straight_line(32-4,32-4,Direction::LEFT,12),
encode_straight_line(32-4,32-5,Direction::UP,11),
encode_straight_line(32-7,32-15,Direction::LEFT,5),
encode_straight_line(32-15,32-7,Direction::UP,5),
            /* top right*/
encode_straight_line(32-13,15,Direction::LEFT,2),
encode_straight_line(32-15,13,Direction::DOWN,2),
encode_straight_line(32-12,12,Direction::LEFT,2),
encode_straight_line(32-6,12,Direction::DOWN,2),
encode_straight_line(32-12,6,Direction::LEFT,2),
encode_straight_line(32-4,4,Direction::LEFT,12),
encode_straight_line(32-4,5,Direction::DOWN,11),
encode_straight_line(32-7,15,Direction::LEFT,5),
encode_straight_line(32-15,7,Direction::DOWN,5),
            /* bot left */
encode_straight_line(13,32-15,Direction::RIGHT,2),
encode_straight_line(15,32-13,Direction::UP,2),
encode_straight_line(12,32-12,Direction::RIGHT,2),
encode_straight_line(6,32-12,Direction::UP,2),
encode_straight_line(12,32-6,Direction::RIGHT,2),
encode_straight_line(4,32-4,Direction::RIGHT,12),
encode_straight_line(4,32-5,Direction::UP,11),
encode_straight_line(7,32-15,Direction::RIGHT,5),
encode_straight_line(15,32-7,Direction::UP,5),
            /* other */
encode_straight_line(15,2,Direction::RIGHT,3),
encode_straight_line(2,15,Direction::DOWN,3),
encode_straight_line(30,15,Direction::DOWN,3)
                )};
/* clang-format on */
inline constexpr std::array level_29_structures{
    Structure{.type = StructureType::POINT,
              .len = 27,
              .data = std::data(level_29_point_data)},
    Structure{.type = StructureType::STRAIGHT_LINE,
              .len = 39,
              .data = std::data(level_29_line_data)}};
inline constexpr Level level_29{.len = std::size(level_29_structures),
                                .data = std::data(level_29_structures)};

/* clang-format off */
static constexpr auto level_30_rect_data{
    embp::concat(
encode_rectangle(1,4,2,5),
encode_rectangle(4,2,5,4),
encode_rectangle(8,1,9,3),
encode_rectangle(11,2,12,3),
encode_rectangle(14,1,15,2),
encode_rectangle(18,1,19,3),
encode_rectangle(22,3,23,4),
encode_rectangle(25,2,26,3),
encode_rectangle(29,4,30,5),
encode_rectangle(2,7,3,8),
encode_rectangle(8,5,9,6),
encode_rectangle(13,5,14,6),
encode_rectangle(16,5,17,7),
encode_rectangle(19,6,20,7),
encode_rectangle(24,5,25,6),
encode_rectangle(5,9,6,10),
encode_rectangle(8,8,9,12),
encode_rectangle(11,8,12,11),
encode_rectangle(14,10,15,13),
encode_rectangle(17,9,18,12),
encode_rectangle(18,13,20,14),
encode_rectangle(20,10,21,11),
encode_rectangle(23,8,24,9),
encode_rectangle(26,8,27,9),
encode_rectangle(28,7,29,9),
encode_rectangle(1,11,3,12),
encode_rectangle(5,15,9,16),
encode_rectangle(11,13,12,15),
encode_rectangle(14,15,15,16),
encode_rectangle(25,12,28,13),
encode_rectangle(30,12,31,13),
encode_rectangle(1,17,3,18),
encode_rectangle(5,18,7,19),
encode_rectangle(9,19,10,20),
encode_rectangle(11,17,12,18),
encode_rectangle(14,18,15,19),
encode_rectangle(17,18,18,19),
encode_rectangle(20,16,21,17),
encode_rectangle(20,19,21,20),
encode_rectangle(23,16,24,17),
encode_rectangle(25,19,26,21),
encode_rectangle(27,16,28,18),
encode_rectangle(30,16,31,17),
encode_rectangle(30,20,31,21),
encode_rectangle(4,21,6,22),
encode_rectangle(8,22,9,24),
encode_rectangle(11,22,12,23),
encode_rectangle(14,21,15,23),
encode_rectangle(17,23,19,24),
encode_rectangle(22,23,24,24),
encode_rectangle(28,23,29,24),
encode_rectangle(3,24,5,25),
encode_rectangle(4,27,6,28),
encode_rectangle(8,26,9,28),
encode_rectangle(13,25,15,26),
encode_rectangle(17,26,18,27),
encode_rectangle(23,28,24,29),
encode_rectangle(25,26,26,27),
encode_rectangle(28,26,29,27),
encode_rectangle(27,29,28,30),
encode_rectangle(3,30,5,31),
encode_rectangle(11,28,12,29),
encode_rectangle(17,30,18,31)
                )};
static constexpr auto level_30_line_data{
    embp::concat(
encode_straight_line(1,23,Direction::DOWN,6),
encode_straight_line(14,29,Direction::DOWN,3),
encode_straight_line(21,27,Direction::DOWN,3),
encode_straight_line(31,27,Direction::DOWN,3)
    )
};
/* clang-format on */
inline constexpr std::array level_30_structures{
    Structure{.type = StructureType::STRAIGHT_LINE,
              .len = 4,
              .data = std::data(level_30_line_data)},
    Structure{.type = StructureType::RECT,
              .len = 63,
              .data = std::data(level_30_rect_data)}};
inline constexpr Level level_30{.len = std::size(level_30_structures),
                                .data = std::data(level_30_structures)};

inline constexpr std::array levels{
    level_1,  level_2,  level_3,  level_4,  level_5,  level_6,
    level_7,  level_8,  level_9,  level_10, level_11, level_12,
    level_13, level_14, level_15, level_16, level_17, level_18,
    level_19, level_20, level_21, level_22, level_23, level_24,
    level_25, level_26, level_27, level_28, level_29, level_30,
};

/* ==============================================================



 * ============================================================== */

} // namespace snake

#endif