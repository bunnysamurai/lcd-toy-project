#include "bsio.hpp"

#include <array>
#include <cstddef>
#include <cstdint>

#include "console/TextOut.hpp"
#include "console/TileBuffer.hpp"
#include "console/glyphs/letters.hpp"
#include "keyboard/keyboard.hpp"
#include "screen/screen.hpp"

#include "pico/stdio.h"
#include "pico/stdio/driver.h"

namespace {

inline constexpr screen::Format CONSOLE_FORMAT{screen::Format::GREY1};
inline constexpr uint8_t TEXTBPP{screen::bitsizeof(CONSOLE_FORMAT)};
inline constexpr screen::Format COLOR_FORMAT{screen::Format::RGB565};
inline constexpr uint8_t COLORBPP{screen::bitsizeof(COLOR_FORMAT)};
inline constexpr uint32_t DISPLAY_WIDTH{screen::PHYSICAL_WIDTH_PIXELS};
inline constexpr uint32_t DISPLAY_HEIGHT{screen::PHYSICAL_HEIGHT_PIXELS};
inline constexpr size_t BUFLEN{DISPLAY_WIDTH * DISPLAY_HEIGHT *
                               MAX_SUPPORTED_BPP / 8};

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
TileBuffer<DISPLAY_WIDTH, DISPLAY_HEIGHT, TEXTBPP, BUFLEN> tile_buf_1bpp{
    frame_buffer};
TileBuffer<DISPLAY_WIDTH, DISPLAY_HEIGHT, 2, BUFLEN> tile_buf_2bpp{
    frame_buffer};
TileBuffer<DISPLAY_WIDTH, DISPLAY_HEIGHT, 4, BUFLEN> tile_buf_4bpp{
    frame_buffer};
TileBuffer<DISPLAY_WIDTH, DISPLAY_HEIGHT, 8, BUFLEN> tile_buf_8bpp{
    frame_buffer};
TileBuffer<DISPLAY_WIDTH, DISPLAY_HEIGHT, COLORBPP, BUFLEN> tile_buf_16bpp{
    frame_buffer};

TextOut<decltype(tile_buf_1bpp)> wrt{tile_buf_1bpp};

/* callbacks for stdio_driver_t */
void my_out_chars(const char *buf, int len) {
  if (screen::get_format() != screen::Format::GREY1) {
    return;
  }
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
      {.width = DISPLAY_WIDTH, .height = DISPLAY_HEIGHT}, CONSOLE_FORMAT)};
  if (!status) {
    return status;
  }
  stdio_init_mine();
  return status;
}

/* =========================================================== */
/*                  Video-Only Mode                            */
/* =========================================================== */

void draw_tile(uint32_t xpos, uint32_t ypos, Tile tile) {
  if (screen::get_format() != tile.format) {
    return;
  }

  switch (tile.format) {
  case screen::Format::GREY1:
    draw(tile_buf_1bpp, tile, xpos, ypos);
    break;
  case screen::Format::GREY2:
    draw(tile_buf_2bpp, tile, xpos, ypos);
    break;
  case screen::Format::GREY4:
    draw(tile_buf_4bpp, tile, xpos, ypos);
    break;
  case screen::Format::RGB565_LUT8:
    draw(tile_buf_8bpp, tile, xpos, ypos);
    break;
  case screen::Format::RGB565:
    draw(tile_buf_16bpp, tile, xpos, ypos);
    break;
  }
}

/* =========================================================== */
/*                   Text-Only Mode                            */
/* =========================================================== */
struct ConsoleConfig {
  uint16_t columns;
  uint16_t lines;
  uint16_t char_width;
  uint16_t char_height;
};
static constexpr ConsoleConfig g_console_cfg{
    .columns = screen::PHYSICAL_WIDTH_PIXELS / glyphs::tile::width(),
    .lines = screen::PHYSICAL_HEIGHT_PIXELS / glyphs::tile::height(),
    .char_width = glyphs::tile::width(),
    .char_height = glyphs::tile::height()};

void set_console_mode() {

  /* Set screen back to 1bpp mode and clear */
  screen::set_format(screen::Format::GREY1);
}
void clear_console() { clear(wrt); }
screen::Dimensions get_console_width_and_height() noexcept {
  return {.width = g_console_cfg.columns, .height = g_console_cfg.lines};
}

void draw_letter(uint32_t column, uint32_t line, char c) {
  const auto tile{glyphs::tile::decode_ascii(c)};
  const auto xpos{column * g_console_cfg.char_width};
  const auto ypos{line * g_console_cfg.char_height};
  draw(tile_buf_1bpp, tile, xpos, ypos);
}
} // namespace bsio
