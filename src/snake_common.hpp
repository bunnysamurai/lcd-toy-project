#if !defined(SNAKE_COMMON_HPP)
#define SNAKE_COMMON_HPP

#include <cstdint>

namespace snake {

using grid_t = uint8_t;
using pix_t = uint16_t;

enum struct Direction : uint8_t {
  UP = 0x0,
  RIGHT = 0x1,
  DOWN = 0x2,
  LEFT = 0x3
};

}
#endif
