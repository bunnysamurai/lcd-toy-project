#if !defined(GAMEPAD_HPP)
#define GAMEPAD_HPP

#include <cstdint>

namespace gamepad::five {

struct State {
  uint8_t up : 1;
  uint8_t down : 1;
  uint8_t right : 1;
  uint8_t left : 1;
  uint8_t etc : 1;
  uint8_t reserved : 3;
};

/* we are initialized by bsio::init() */
/* just 5 buttons, all direct GPIOs */
void init() noexcept;
void deinit() noexcept;
[[nodiscard]] State get() noexcept;

} // namespace gamepad::five

namespace gamepad::eight {

struct State {
  uint8_t up : 1;
  uint8_t down : 1;
  uint8_t right : 1;
  uint8_t left : 1;
  uint8_t start : 1;
  uint8_t select : 1;
  uint8_t abutton : 1;
  uint8_t button : 1;
};

/* we are initialized by bsio::init() */
/* we are going to do similar to the NES gamepad, and that's read an 8bit shift
 * register (74xx165) using PIO */

[[nodiscard]] State get() noexcept;
} // namespace gamepad::eight
#endif