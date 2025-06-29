#if !defined(TINYUSBKEYBOARD_HPP)
#define TINYUSBKEYBOARD_HPP

#include "keyboard.hpp"

namespace keyboard::bsp::tinyusb {
[[nodiscard]] char wait_key(duration, result_t &) noexcept;
} // namespace keyboard::bsp
#endif