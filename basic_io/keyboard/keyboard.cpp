#include "keyboard.hpp"

#ifdef TINYUSB_BASICKEYBOARD
#include "TinyUsbKeyboard.hpp"
#endif

namespace keyboard {

#ifdef TINYUSB_BASICKEYBOARD
[[nodiscard]] char wait_key(duration timeout, result_t &err) noexcept {
  return keyboard::bsp::tinyusb::wait_key(timeout, err);
}
#endif

} // namespace keyboard