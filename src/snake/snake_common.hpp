#if !defined(SNAKE_COMMON_HPP)
#define SNAKE_COMMON_HPP

#include <cstdint>
#include "common/Grid.hpp"

namespace snake {

using grid_t = uint8_t;
using pix_t = uint16_t;

inline constexpr grid_t PLAY_SIZE{31U};

enum struct Direction : uint8_t {
  UP = 0x0,
  RIGHT = 0x1,
  DOWN = 0x2,
  LEFT = 0x3
};

} // namespace snake
#endif
