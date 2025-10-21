#if !defined(REVENGED_DEFS_HPP)
#define REVENGED_DEFS_HPP

#include <cstdint>

#include "common/BitImage.hpp"
#include "common/Grid.hpp"
#include "common/pico_sdk_timer_details.hpp"
#include "common/timer.hpp"

namespace revenge {

using Timer_t = Timer<timer_details::PicoSdk>;
using pix_t = uint16_t;
using grid_t = uint8_t;

inline constexpr Timer_t::time_diff_t GAME_TICK_PERIOD_US{1'500'000};
inline constexpr Timer_t::time_diff_t STUCK_IN_HOLE_PERIOD_US{5'000'000};
inline constexpr Timer_t::time_diff_t INPUT_POLL_PERIOD_US{10'000};

inline constexpr uint8_t GRID_SIZE_COLS{21};
inline constexpr uint8_t GRID_SIZE_ROWS{21};
inline constexpr uint8_t PIXELS_PER_GRID{10};

inline constexpr uint32_t MAX_NUMBER_OF_CATS{16};

using PlayGrid = ThreeBitImage<GRID_SIZE_COLS, GRID_SIZE_ROWS>;

/** @brief Types of collisions
 */
enum struct Collision {
  NONE = 0,
  BLOCK = 1,
  TRAP = 2,
  HOLE = 3,
  FIXED_BLOCK = 4,
  CHEESE = 5,
  CAT = 6,
  MOUSE = 7
};

/** @brief Types of objects on the field, that don't move by themselves
 */
enum struct GridObject {
  NOTHING = 0b000,
  MOVABLE_BLOCK = 0b001,
  TRAP = 0b010,
  HOLE = 0b011,
  UNMOVEABLE_BLOCK = 0b100,
  CHEESE = 0b101
};

/** @brief the directions things can move
 */
enum struct Direction {
  UP,
  DOWN,
  LEFT,
  RIGHT,
  UP_AND_LEFT,
  UP_AND_RIGHT,
  DOWN_AND_LEFT,
  DOWN_AND_RIGHT
};

/** @brief Possible user inputs */
enum struct UserInput {
  MOVE_UP,
  MOVE_DOWN,
  MOVE_LEFT,
  MOVE_RIGHT,
  NO_ACTION,
  QUIT
};

/** @brief to make iterating over Direction easier */
static constexpr std::array<Direction, 8> DIRECTIONS_ARRAY{
    Direction::UP,
    Direction::DOWN,
    Direction::LEFT,
    Direction::RIGHT,
    Direction::UP_AND_LEFT,
    Direction::UP_AND_RIGHT,
    Direction::DOWN_AND_LEFT,
    Direction::DOWN_AND_RIGHT};
[[nodiscard]] constexpr Grid::Location move(Grid::Location loc,
                                            Direction dir) noexcept {
  switch (dir) {
  case Direction::UP:
    return {.x = loc.x, .y = loc.y - 1};
  case Direction::DOWN:
    return {.x = loc.x, .y = loc.y + 1};
  case Direction::LEFT:
    return {.x = loc.x - 1, .y = loc.y};
  case Direction::RIGHT:
    return {.x = loc.x + 1, .y = loc.y};
  case Direction::UP_AND_LEFT:
    return {.x = loc.x - 1, .y = loc.y + 1};
  case Direction::UP_AND_RIGHT:
    return {.x = loc.x + 1, .y = loc.y + 1};
  case Direction::DOWN_AND_LEFT:
    return {.x = loc.x - 1, .y = loc.y - 1};
  case Direction::DOWN_AND_RIGHT:
    return {.x = loc.x + 1, .y = loc.y - 1};
  }
  return loc;
}

class Beast final {
private:
  Grid::Location m_location;

public:
  explicit constexpr Beast() noexcept = default;
  explicit constexpr Beast(Grid::Location start) noexcept : m_location{start} {}

  constexpr void move(Direction dir) noexcept { m_location = proposed(dir); }

  [[nodiscard]] constexpr Grid::Location
  proposed(Direction dir) const noexcept {
    return revenge::move(m_location, dir);
  }
  [[nodiscard]] constexpr Grid::Location location() const noexcept {
    return m_location;
  }

  constexpr void location(Grid::Location new_location) noexcept {
    m_location = new_location;
  }
};

} // namespace revenge

#endif