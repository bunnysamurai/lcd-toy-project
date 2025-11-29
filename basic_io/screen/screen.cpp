#include "screen.hpp"
#include "screen_def.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>

// #define PRINT_DEBUG
#ifdef PRINT_DEBUG
#include "pico/printf.h"
#endif

#if defined(WAVESHARE_240P)
#include "waveshare_driver/screen_impl.hpp"
#include "waveshare_driver/screensize.h"
#endif

#include "TileBuffer.hpp"
#include "glyphs/letters.hpp"

namespace {

/* if you want full resolution, can't go over 4 bits per pixel*/
inline constexpr size_t CONFIGURED_MAX_BPP{4};
static_assert(CONFIGURED_MAX_BPP < MAX_SUPPORTED_BPP);

inline constexpr screen::Format CONSOLE_FORMAT{screen::Format::GREY1};
inline constexpr uint8_t TEXTBPP{screen::bitsizeof(CONSOLE_FORMAT)};
inline constexpr screen::Format COLOR_FORMAT{screen::Format::RGB565};
inline constexpr uint8_t COLORBPP{screen::bitsizeof(COLOR_FORMAT)};
inline constexpr uint32_t DISPLAY_WIDTH{screen_impl::PHYSICAL_WIDTH_PIXELS};
inline constexpr uint32_t DISPLAY_HEIGHT{screen_impl::PHYSICAL_HEIGHT_PIXELS};
inline constexpr size_t BUFLEN{DISPLAY_WIDTH * DISPLAY_HEIGHT *
                               CONFIGURED_MAX_BPP / 8};

bool g_video_is_initd{false};

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

TileBuffer<DISPLAY_WIDTH, DISPLAY_HEIGHT, 1, BUFLEN> tile_buf_1bpp{
    frame_buffer};
TileBuffer<DISPLAY_WIDTH, DISPLAY_HEIGHT, 2, BUFLEN> tile_buf_2bpp{
    frame_buffer};
TileBuffer<DISPLAY_WIDTH, DISPLAY_HEIGHT, 4, BUFLEN> tile_buf_4bpp{
    frame_buffer};
TileBuffer<DISPLAY_WIDTH, DISPLAY_HEIGHT / 2, 8, BUFLEN> tile_buf_8bpp{
    frame_buffer};
TileBuffer<DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2, 16, BUFLEN> tile_buf_16bpp{
    frame_buffer};

} // namespace

namespace screen {

uint32_t get_buf_len() { return BUFLEN; }

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

uint8_t *get_video_buffer() noexcept { return std::data(frame_buffer); }

bool init(Position virtual_topleft, Dimensions virtual_size,
          Format format) noexcept {
  /* check if we are already init'd */
  if (!g_video_is_initd) {
    g_video_is_initd = true;
    return screen_impl::init(std::data(frame_buffer), virtual_topleft,
                             virtual_size, format);
  }
  set_format(format);
  set_virtual_screen_size(virtual_topleft, virtual_size);
  return true;
}

void init_clut(const Clut *entries, uint32_t length) noexcept {
  screen_impl::init_clut(entries, length);
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
  /* 255-as-black appears to be screen specific? */
  switch (screen::get_format()) {
  case screen::Format::GREY1:
  case screen::Format::GREY2:
  case screen::Format::GREY4:
  case screen::Format::RGB565:
  case screen::Format::RGB565_LUT4:
  case screen::Format::RGB565_LUT8:
    fill_screen(0x00);
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
  case screen::Format::RGB565_LUT4:
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
void draw_tile_with_replacement(uint32_t xpos, uint32_t ypos, Tile tile,
                                uint32_t pattern, uint32_t replacement) {}

void fill_screen(uint32_t raw_value) {
  auto *p_vbuf{std::data(frame_buffer)};
  memset(p_vbuf, static_cast<uint8_t>(raw_value), std::size(frame_buffer));
  // for (auto &pix : frame_buffer) {
  //   pix = static_cast<uint8_t>(raw_value);
  // }
}

/** @brief compute the byte offset into a row */
[[nodiscard]] static constexpr uint32_t
compute_column_byte_offset(const screen::Format fmt,
                           uint32_t pixel_offset) noexcept {
  switch (fmt) {
  case screen::Format::GREY1:
    return pixel_offset >> 3;
  case screen::Format::GREY2:
    return pixel_offset >> 2;
  case screen::Format::GREY4:
  case screen::Format::RGB565_LUT4:
    return pixel_offset >> 1;
  case screen::Format::RGB565_LUT8:
    return pixel_offset;
  case screen::Format::RGB565:
    return pixel_offset << 1;
  }

  /* this is technically unreachable if warnings are cranked up high enough. */
  return pixel_offset;
}

/** @brief get pointer to the start of row */
[[nodiscard]] static constexpr uint8_t *
get_start_of_row(uint8_t *p_start_of_frame, const screen::Format fmt,
                 uint32_t pixrow_offset, Dimensions dims) noexcept {
  const auto row_length_bytes{compute_column_byte_offset(fmt, dims.width)};
  return std::next(p_start_of_frame, row_length_bytes * pixrow_offset);
}

void fillrows(uint32_t value, uint32_t row_start, uint32_t row_finish,
              uint32_t column_start, uint32_t column_finish) {

  /* a whole bunch of setup stuff... */
  const auto dims{get_virtual_screen_size()};

  if (column_start >= dims.width || column_start >= column_finish) {
    return;
  }
  column_finish = {column_finish < dims.width ? column_finish : dims.width};

  const auto fmt{screen::get_format()};

  const auto byte_start{compute_column_byte_offset(fmt, column_start)};
  const auto byte_finish{compute_column_byte_offset(fmt, column_finish)};
  const auto pitch{compute_column_byte_offset(fmt, dims.width)};
  auto *p_row{
      get_start_of_row(screen::get_video_buffer(), fmt, row_start, dims)};
  const auto *const p_row_end{
      get_start_of_row(screen::get_video_buffer(), fmt, row_finish, dims)};

  /* now we get down to business */
  if (fmt == screen::Format::RGB565) {
    /* hand roll this case */
    while (p_row != p_row_end) {
      for (uint32_t idx = 0; idx < (byte_finish - byte_start); idx += 2) {
        p_row[idx >> 1] = value & 0xff;
        p_row[(idx >> 1) + 1] = (value >> 8) & 0xff;
      }
      std::advance(p_row, pitch);
    }
  } else {
    /* expand the replacement value as appropriate, then memset */
    uint8_t expanded{};
    switch (fmt) {
    case screen::Format::GREY1:
      expanded = (value & 0b1) | (value & 0b1) << 1 | (value & 0b1) << 2 |
                 (value & 0b1) << 3 | (value & 0b1) << 4 | (value & 0b1) << 5 |
                 (value & 0b1) << 6 | (value & 0b1) << 7;
      break;
    case screen::Format::GREY2:
      expanded = (value & 0b11) | (value & 0b11) << 2 | (value & 0b11) << 4 |
                 (value & 0b11) << 6;
      break;
    case screen::Format::GREY4:
    case screen::Format::RGB565_LUT4:
      expanded = (value & 0b1111) | (value & 0b1111) << 4;
      break;
    case screen::Format::RGB565_LUT8:
      expanded = value;
      break;
    case screen::Format::RGB565:
      return; /* this should not be possible to reach */
    }
    while (p_row != p_row_end) {
      memset(std::next(p_row, byte_start), expanded, byte_finish - byte_start);
      std::advance(p_row, pitch);
    }
  }
}

void copyrow(const uint32_t dst, const uint32_t src, uint32_t column_start,
             uint32_t column_finish) {

  const auto dims{get_virtual_screen_size()};

  if (column_start >= dims.width || column_start >= column_finish) {
    return;
  }
  column_finish = {column_finish < dims.width ? column_finish : dims.width};

  /* need to correct offsets, which depends on the screen format */

  const auto start{
      compute_column_byte_offset(screen::get_format(), column_start)};
  const auto finish{
      compute_column_byte_offset(screen::get_format(), column_finish)};

  auto *p_dst{get_start_of_row(screen::get_video_buffer(), screen::get_format(),
                               dst, dims)};
  auto *p_src{get_start_of_row(screen::get_video_buffer(), screen::get_format(),
                               src, dims)};

  memcpy(std::next(p_dst, start), std::next(p_src, start), finish - start);
}

void melt(uint32_t replacement_value) {
  /* this is an interesting one
   * concept is:
   *    for each column, scroll it down by x pixels
   *    in addition, each column only starts scrolling at a specific time
   *    this is an animation, after all, and we should have a little bit
   *      of acceleration on the scrolling until it reaches a terminal
   *      velocity.
   *
   *  I guess we'll use a timer to know when to update the framebuffer
   *  We'll use an array to manage what each columns' current velocity is.
   *    This array will be initialized with random velocities.
   *  We'll have a constant to use for max velocity
   *
   *  When the set time is expired, we will:
   *    update the column velocity vector
   *    compute the distance traveled for each column
   *      this is simply the velocity times the timer period
   *    scroll the column by that distance
   *    replace the moved-from positions with the replacement value.
   */
  static constexpr uint32_t FIXED_SCROLL{1U};
  const auto dims{get_virtual_screen_size()};
  const auto fmt{screen::get_format()};

  auto &&scroll{[&](auto colstrt, auto colend) {
    uint32_t scroll_start{dims.height - 1 - FIXED_SCROLL};
    for (;;) {
      // assign row to the one directly beneath
      copyrow(scroll_start + 1, scroll_start, colstrt, colend);
      if (scroll_start == 0) {
        break;
      }
      --scroll_start;
    }
    /* fill the rows that weren't mutated with the replacement value */
    fillrows(replacement_value, FIXED_SCROLL - 1, FIXED_SCROLL, colstrt,
             colend);
  }};

  /* we'll try 8 different sections */
  static constexpr uint32_t SECLEN{3};
  static constexpr uint32_t SECARRLEN{1 << SECLEN};
  std::array<uint16_t, SECARRLEN> position{};
  std::array<uint8_t, SECARRLEN> counter{};
  // std::array<uint8_t, SECARRLEN> velocity{24, 22, 20, 18, 16, 14, 12, 10,
  //                                         12, 14, 16, 18, 20, 22, 24, 26};
  std::array<uint8_t, SECARRLEN> velocity{24, 20, 16, 12, 14, 18, 22, 26};
  const auto secwidth{dims.width >> SECLEN};

  for (;;) {
    // sleep_us(10);
    bool we_are_done{true};
    for (uint32_t idx = 0; idx < SECARRLEN; ++idx) {
      const bool done{position[idx] == dims.height};
      we_are_done &= done;
      if (!done && counter[idx]++ == velocity[idx]) {
        const auto colstrt{idx * secwidth};
        const auto colend{(idx + 1) * secwidth};
        scroll(colstrt, colend);
        ++position[idx];
        counter[idx] = 0;
      }
    }
    if (we_are_done) {
      break;
    }
  }
}

/** @brief Change a pixel in memory, format-aware */
void poke(uint32_t xpos, uint32_t ypos, uint32_t value) noexcept {

  const auto fmt{get_format()};
  const auto dims{get_virtual_screen_size()};
  const auto rowstart{ypos * dims.width};
  const auto linidx{xpos + rowstart};
  const uint32_t mask{(1 << bitsizeof(fmt)) - 1};
  const auto byteidx{align_byte(linidx, fmt)};

  const auto shift{screen::subbyte_index(linidx, fmt) * screen::bitsizeof(fmt)};
  const uint8_t setval{value << shift};
  const uint32_t clearmask{~(mask << shift)};

  auto *pbuf{get_video_buffer()};

  /* op is to clear the relevant bits, then set the relevant bits */
  pbuf[byteidx] &= clearmask;
  pbuf[byteidx] |= setval;
}

/** @brief Read a pixel in memory, format-aware */
uint32_t peek(uint32_t xpos, uint32_t ypos) noexcept {

  const auto fmt{get_format()};
  const auto dims{get_virtual_screen_size()};
  const auto rowstart{ypos * dims.width};
  const auto linidx{xpos + rowstart};
  const uint32_t mask{(1 << bitsizeof(fmt)) - 1};
  const auto byteidx{align_byte(linidx, fmt)};

  const auto shift{screen::subbyte_index(linidx, fmt) * screen::bitsizeof(fmt)};
  const uint32_t clearmask{mask << shift};

  const auto *pbuf{get_video_buffer()};

  return (pbuf[byteidx] & clearmask) >> shift;
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

bool set_console_mode() noexcept {
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
   * which we just set to whitespace
   * I also assume we are in 1bpp mode.  If not, we exit early.  A future
   * feature.
   */

  auto *vbuf{std::data(frame_buffer)};
  const auto dims{screen::get_console_width_and_height()};
  const auto scroll_height_pix{glyphs::tile::height()};
  const auto width_pix{screen::get_virtual_screen_size().width};
  const auto pitch_pix{scroll_height_pix * width_pix};

  if (screen::get_format() != screen::Format::GREY1) {
    return;
  }

  if (lines <= 0) {
    return;
  }

  if (lines >= dims.height) {
    clear_console();
  }

  int line = lines;
  for (; line < dims.height; ++line) {
    const auto idx{line * pitch_pix};
    const auto prev_idx{idx - (pitch_pix * lines)};

    /* TODO inner loop could be optimized?  The lines should be far enough
     * apart that overlap isn't possible... Better measure! */
    for (int xx = 0; xx < pitch_pix; xx += 8) {
      vbuf[(prev_idx + xx) >> 3] = vbuf[(idx + xx) >> 3];
    }
  }

  line = dims.height - lines;
  for (; line < dims.height; ++line) {
    for (int col = 0; col < width_pix; ++col) {
      draw_letter(col, line, ' ');
    }
  }
}
} // namespace screen
