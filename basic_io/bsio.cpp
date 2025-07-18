#include "bsio.hpp"
#include <array>
#include <cstddef>
#include <cstdint>

#include "TextConsole.hpp"
#include "keyboard/keyboard.hpp"
#include "screen/TileBuffer.hpp"
#include "screen/screen.hpp"

#include "pico/stdio.h"
#include "pico/stdio/driver.h"
#if defined(BUILD_WITH_STDIO_USB)
#include "pico/stdio_usb.h"
#endif

namespace {

#if !defined(BUILD_WITH_STDIO_USB)
screen::TextConsole wrt;

/* callbacks for stdio_driver_t */
void my_out_chars(const char *buf, int len) {
  if (screen::get_format() != screen::Format::GREY1) {
    return;
  }
  for (int ii = 0; ii < len; ++ii) {
    wrt.putc(buf[ii]);
  }
}
void my_out_flush() { /* do nothing, we don't buffer... right meow */ }
int my_in_chars(char *buf, int len) {
  /* we block until we get all the keys! */
  keyboard::result_t err;
  for (int ii = 0; ii < len; ++ii) {
    const char c{keyboard::wait_key(std::chrono::milliseconds{1}, err)};
    if (err != keyboard::result_t::SUCCESS) {
      return ii;
    }
    buf[ii] = c;
  }
  return len;
}

void stdio_init_mine() {
  static stdio_driver_t my_driver = {.out_chars = my_out_chars,
                                     .out_flush = my_out_flush,
                                     .in_chars = my_in_chars};
  stdio_set_driver_enabled(&my_driver, true);
}
#endif

} // namespace

namespace bsio {

bool init() {
  if (!screen::set_console_mode()) {
    return false;
  }
#if defined(BUILD_WITH_STDIO_USB)
  stdio_usb_init();
#else
  stdio_init_mine();
#endif
  return true;
}

} // namespace bsio
