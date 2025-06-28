#include "bsio.hpp"

#include <array>
#include <cstddef>
#include <cstdint>

#include "console/TextOut.hpp"
#include "console/TileBuffer.hpp"
#include "keyboard/TinyUsbKeyboard.hpp"
#include "screen/screen.hpp"

#include "pico/stdio.h"
#include "pico/stdio/driver.h"

namespace {

inline constexpr uint8_t BPP{1};
inline constexpr uint32_t DISPLAY_WIDTH{screen::PHYSICAL_SIZE.width};
inline constexpr uint32_t DISPLAY_HEIGHT{screen::PHYSICAL_SIZE.height};
inline constexpr size_t BUFLEN{DISPLAY_WIDTH * DISPLAY_HEIGHT * BPP / 8};
// pixels per byte is a function of bpp
// need to convert from number of pixels to number of bits, then to number of
// bytes.

template <class T, class U>
constexpr void init_to_all_val(T &buf, const U &val) {
  for (auto &el : buf) {
    el = val;
  }
}

constexpr std::array<uint8_t, BUFLEN> init_the_buffer() {
  std::array<uint8_t, BUFLEN> rv{};
  init_to_all_val(rv, std::numeric_limits<uint8_t>::min());
  return rv;
}

auto frame_buffer{init_the_buffer()};
TileBuffer<DISPLAY_WIDTH, DISPLAY_HEIGHT, BPP> tile_buf{frame_buffer};
TextOut<decltype(tile_buf)> wrt{tile_buf};

/* callbacks for stdio_driver_t */
void my_out_chars(const char *buf, int len) {
  for (int ii = 0; ii < len; ++ii) {
    putc(wrt, buf[ii]);
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

} // namespace

namespace bsio {

bool init() {
  const bool status{screen::init(
      std::data(frame_buffer), {.row = 0, .column = 0},
      {.width = DISPLAY_WIDTH, .height = DISPLAY_HEIGHT}, {.bpp = BPP})};
  if (!status) {
    return status;
  }
  stdio_init_mine();
  return status;
}

void clear_screen() { clear(wrt); }

} // namespace bsio
