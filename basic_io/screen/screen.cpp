#include "screen.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>

#if defined(WAVESHARE_240P)
#include "waveshare_driver/screen_impl.hpp"
#include "waveshare_driver/screensize.h"
#endif

#include "TileBuffer.hpp"
#include "glyphs/letters.hpp"

namespace {

inline constexpr screen::Format CONSOLE_FORMAT{screen::Format::GREY1};
inline constexpr uint8_t TEXTBPP{screen::bitsizeof(CONSOLE_FORMAT)};
inline constexpr screen::Format COLOR_FORMAT{screen::Format::RGB565};
inline constexpr uint8_t COLORBPP{screen::bitsizeof(COLOR_FORMAT)};
inline constexpr uint32_t DISPLAY_WIDTH{screen_impl::PHYSICAL_WIDTH_PIXELS};
inline constexpr uint32_t DISPLAY_HEIGHT{screen_impl::PHYSICAL_HEIGHT_PIXELS};
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
} // namespace

namespace screen {

void set_format(Format fmt) noexcept { screen_impl::set_format(fmt); }

Format get_format() noexcept { return screen_impl::get_format(); }

Dimensions get_virtual_screen_size() noexcept {
  return screen_impl::get_virtual_screen_size();
}

void set_virtual_screen_size([[maybe_unused]] Position new_topleft,
                             Dimensions new_size) noexcept {
  screen_impl::set_virtual_screen_size(new_topleft, new_size);
}

[[nodiscard]] Dimensions get_physical_screen_size() noexcept {
  return {.width = screen_impl::PHYSICAL_WIDTH_PIXELS,
          .height = screen_impl::PHYSICAL_HEIGHT_PIXELS};
}

const uint8_t *get_video_buffer() noexcept {
  return screen_impl::get_video_buffer();
}

bool init([[maybe_unused]] Position virtual_topleft, Dimensions virtual_size,
          Format format) noexcept {
  return screen_impl::init(std::data(frame_buffer), virtual_topleft,
                           virtual_size, format);
}

void set_video_buffer(const uint8_t *buffer) noexcept {
  screen_impl::set_video_buffer(buffer);
}

[[nodiscard]] bool get_touch_report(TouchReport &out) {
  return screen_impl::get_touch_report(out);
}

/* =========================================================== */
/*                  Video-Only Mode                            */
/* =========================================================== */
void clear_screen() {
  /* just set all bytes to 0 */
  for (auto &pix : frame_buffer) {
    pix = 0U;
  }
}

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
    .columns = screen_impl::PHYSICAL_WIDTH_PIXELS / glyphs::tile::width(),
    .lines = screen_impl::PHYSICAL_HEIGHT_PIXELS / glyphs::tile::height(),
    .char_width = glyphs::tile::width(),
    .char_height = glyphs::tile::height()};

bool set_console_mode() {
  const bool status{init({.row = 0, .column = 0}, get_physical_screen_size(),
                         screen::Format::GREY1)};
  if (!status) {
    return status;
  }
  clear_console();
  return status;
}

void clear_console() {
  /* set to all whitespace */
  for (size_t yy = 0; yy < g_console_cfg.lines; ++yy) {
    for (size_t xx = 0; xx < g_console_cfg.columns; ++xx) {
      draw_letter(xx, yy, ' ');
    }
  }
}

screen::Dimensions get_console_width_and_height() noexcept {
  return {.width = g_console_cfg.columns, .height = g_console_cfg.lines};
}

void draw_letter(uint32_t column, uint32_t line, char c) {
  const auto tile{glyphs::tile::decode_ascii(c)};
  const auto xpos{column * g_console_cfg.char_width};
  const auto ypos{line * g_console_cfg.char_height};
  draw(tile_buf_1bpp, tile, xpos, ypos);
}

void scroll_up(int lines) {

  /*
   *  | text .... |
   *  | test .... |
   *  | best .... |
   *
   *   Scrolling up
   *       |
   *       V
   *  | test .... |
   *  | best .... |
   *  |           |
   */

  /* so, we start at the second line and copy it to the first
   * then repeat until we get to the last line,
   * which we just set to whitespace */

  auto *vbuf{std::data(frame_buffer)};
  const auto dims{screen::get_console_width_and_height()};
  const auto scroll_height_pix{glyphs::tile::height()};

  for (int line = 1; line < dims.height; ++line) {
    const auto width_pix{dims.width * glyphs::tile::width()};
    const auto idx{(line * scroll_height_pix) * width_pix};
    const auto prev_idx{((line - 1) * scroll_height_pix) * width_pix};

    /* TODO inner loop could be optimized?  The lines should be far enough apart
     * that overlap isn't possible... Better measure! */
    for (int xx = 0; xx < width_pix; ++xx) {
      vbuf[prev_idx + xx] = vbuf[idx + xx];
    }
  }
}
} // namespace screen
