#include "demo.hpp"

#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <string_view>

#include "pico/printf.h"
#include "pico/rand.h"
#include "pico/stdio.h"
#include "pico/time.h"

#include "bsio.hpp"

#include "console/TileDef.h"
#include "screen/screen.hpp"

namespace demo {

enum struct Color {
  WHITE,
  BLACK,
  GREY,
  RED,
  GREEN,
  BLUE,
  MAGENTA,
  CYAN,
  YELLOW
};
[[nodiscard]] constexpr uint16_t to_rgb565(Color clr) noexcept {
  switch (clr) {
  case Color::WHITE:
    return 0xFFFFU;
  case Color::BLACK:
    return 0x0000U;
  case Color::GREY:
    return 0xFFFFU >> 1;
  case Color::RED:
    return 0b11111'000000'00000;
  case Color::GREEN:
    return 0b00000'111111'00000;
  case Color::BLUE:
    return 0b00000'000000'11111;
  case Color::MAGENTA:
    return 0b11111'000000'11111;
  case Color::CYAN:
    return 0b00000'111111'11111;
  case Color::YELLOW:
    return 0b11111'111111'00000;
  }
  return 0;
}

void fill_routine(const Tile &tile) {
  const auto dims{screen::get_virtual_screen_size()};
  for (size_t yy = 0; yy < dims.height; yy += tile.side_length) {
    for (size_t xx = 0; xx < dims.width; xx += tile.side_length) {
      bsio::draw_tile(xx, yy, tile);
    }
  }
}

template <size_t N>
[[nodiscard]] constexpr std::array<uint8_t, N> fill_with(Color clr) noexcept {
  std::array<uint8_t, N> result;
  for (size_t idx{0}; idx < N; idx += 2) {
    const uint16_t bytes{to_rgb565(clr)};
    /* little-endian, I think?? */
    result[idx] = bytes & 0x00FFU;
    result[idx + 1] = (bytes & 0xFF00U) >> 8;
  }
  return result;
}

/* I think 8x8 tiles are okay? */
constexpr auto magenta_data{fill_with<8 * 8 * 2>(Color::MAGENTA)};
constexpr auto gold_data{fill_with<8 * 8 * 2>(Color::YELLOW)};
constexpr auto emerald_data{fill_with<8 * 8 * 2>(Color::CYAN)};
constexpr auto red_data{fill_with<8 * 8 * 2>(Color::RED)};
constexpr auto green_data{fill_with<8 * 8 * 2>(Color::GREEN)};
constexpr auto blue_data{fill_with<8 * 8 * 2>(Color::BLUE)};

constexpr Tile blue{.side_length = 8,
                    .format = screen::Format::RGB565,
                    .data = std::data(blue_data)};
constexpr Tile green{.side_length = 8,
                     .format = screen::Format::RGB565,
                     .data = std::data(green_data)};
constexpr Tile red{.side_length = 8,
                   .format = screen::Format::RGB565,
                   .data = std::data(red_data)};
constexpr Tile magenta{.side_length = 8,
                       .format = screen::Format::RGB565,
                       .data = std::data(magenta_data)};
constexpr Tile gold{.side_length = 8,
                    .format = screen::Format::RGB565,
                    .data = std::data(gold_data)};
constexpr Tile emerald{.side_length = 8,
                       .format = screen::Format::RGB565,
                       .data = std::data(emerald_data)};

void run_color_rando_art() {
  static constexpr std::array<Tile, 4> tilelist{gold, green, gold, gold};
  const auto dims{screen::get_virtual_screen_size()};

  auto &&random_routine{[&]() {
    for (size_t yy = 0; yy < (dims.height >> 1);
         yy += tilelist[0].side_length) {
      for (size_t xx = 0; xx < (dims.width >> 1);
           xx += tilelist[0].side_length) {
        const auto r{get_rand_32() & 0b11};
        const auto &rtile{tilelist[r]};
        bsio::draw_tile(xx, yy, rtile);
        bsio::draw_tile(dims.width - xx - rtile.side_length, yy, rtile);
        bsio::draw_tile(xx, dims.height - yy - rtile.side_length, rtile);
        bsio::draw_tile(dims.width - xx - rtile.side_length,
                        dims.height - yy - rtile.side_length, rtile);
      }
    }
  }};

  bsio::clear_console();
  screen::set_format(screen::Format::RGB565);
  fill_routine(emerald);
  while (true) {
    random_routine();
    sleep_ms(2000);
  }
}

void run_text_animation() {
  bsio::clear_console();
  printf("+------------------+\n");
  printf("| Meven 2040 Demo! |\n");
  printf("+------------------+\n");
  sleep_ms(1000);
  while (true) {
    // run an animation, by hand
    printf("C:\\> ");
    sleep_ms(500);
    static constexpr std::string_view the_stuff{
        R"(This is a story all about how my life got switched turned upside down so take a minute just sit right there while I tell you howibecametheprince of a town called BelAir.)"};
    for (const auto c : the_stuff) {
      printf("%c", c);
      sleep_ms(50);
    }
    sleep_ms(500);
    printf("\n");
    sleep_ms(100);
    printf("Bad batch file or command\n");
    sleep_ms(200);
  }
}

enum struct TouchMachine { WAIT, PEN_DOWN, PEN_UP };

namespace {

void initialize_cool_touch_demo() {
  bsio::clear_console();
  screen::set_format(screen::Format::RGB565);
  fill_routine(emerald);
}

[[nodiscard]] constexpr screen::TouchReport
to_pixelspace(screen::TouchReport rawloc) noexcept {
  /*
   * Screen is 320 columns and 240 rows
   * Translation for our rotated text console is:
   *  bottom row is x~=250
   *  top row is x~=3600
   *  left col is y~=350
   *  right col is y~=3850
   * For the native pixel display (i.e. our tiler):
   *  col 0 is x~=250
   *  col 239 is x~=3600
   *  row 0 is y~=350
   *  row 319 is y~=3850
   *
   * need to solve the linear system:
   *  y = mx + b
   * for both row and col:
   *  col = m_col * rawloc.x + b_col
   *  row = m_row * rawloc.y + b_row
   *
   * for col:
   * |  0  |   | 250   1  |   | m_col |
   * | 239 | = | 3600  1  | * | b_col |
   * for row:
   * |  0  |   | 350   1  |   | m_row |
   * | 319 | = | 3850  1  | * | b_row |
   *
   * using octave-cli ( via A\b ), yields:
   *  m_col = 0.071343
   *  b_col = -17.836
   *  m_row = 0.091143
   *  b_row = -31.9
   * we don't have a floating point unit, so fixed
   * point it is!
   */
  constexpr int32_t M_COL_S_24_8{18};
  constexpr int32_t B_COL_S_24_8{-4548};

  constexpr int32_t M_ROW_S_24_8{23};
  constexpr int32_t B_ROW_S_24_8{-8166};

  auto &&zclamp{[](int16_t val, int16_t hi) -> int16_t {
    if (val < 0) {
      return 0;
    }
    if (val > hi) {
      return hi;
    }
    return val;
  }};

  const auto raw_x{
      (M_COL_S_24_8 * static_cast<int32_t>(rawloc.x) + B_COL_S_24_8) >> 8};
  const auto raw_y{
      (M_ROW_S_24_8 * static_cast<int32_t>(rawloc.y) + B_ROW_S_24_8) >> 8};

  const auto dims{screen::get_virtual_screen_size()};
  return screen::TouchReport{.x = zclamp(raw_x, dims.width - 1),
                             .y = zclamp(raw_y, dims.height - 1),
                             .pen_up = rawloc.pen_up,
                             .timestamp = rawloc.timestamp};
}

void undo_cool_touch_action(screen::TouchReport touch_loc) {
  if (touch_loc.x < 0 || touch_loc.y < 0) {
    return;
  }
  touch_loc = to_pixelspace(touch_loc);
  const auto dims{screen::get_virtual_screen_size()};
  bsio::draw_tile(dims.width - touch_loc.x - 1, dims.height - touch_loc.y - 1,
                  emerald);
}

void take_cool_touch_action(screen::TouchReport touch_loc) {
  touch_loc = to_pixelspace(touch_loc);
  const auto dims{screen::get_virtual_screen_size()};
  bsio::draw_tile(dims.width - touch_loc.x - 1, dims.height - touch_loc.y - 1,
                  red);
}

} // namespace

static constexpr int64_t DOUBLE_TAP_PERIOD_THRESHOLD_US{550 *
                                                        1000}; /* 100 ms? */
void run_touch_demo(TouchConfig cfg) {
  /* sample about every 10 ms?
   * we should really setup a timer for this... */
  const auto TOUCH_POLL_INTERVAL_MS{cfg.touch_poll_interval_ms};
  dispConfigureTouch(cfg.touchcfg);

  initialize_cool_touch_demo();

  // screen::TouchReport touch_to_undo{.x = -1};
  absolute_time_t time_since_last_pen_up{0};
  screen::TouchReport touch_since_last_pen_up{};
  screen::TouchReport touch;
  screen::TouchReport prv_touch;
  TouchMachine state{TouchMachine::WAIT};
  while (true) {
    switch (state) {
    case TouchMachine::WAIT:
      if (screen::get_touch_report(touch) && !touch.pen_up) {
        state = TouchMachine::PEN_DOWN;
        prv_touch = touch;
      }
      break;
    case TouchMachine::PEN_DOWN:
      if (screen::get_touch_report(touch)) {
        if (touch.pen_up) {
          state = TouchMachine::PEN_UP;
        } else {
          prv_touch = touch;
          take_cool_touch_action(prv_touch);
        }
      }
      break;
    case TouchMachine::PEN_UP:
      const auto now{get_absolute_time()};
      const auto timediff{absolute_time_diff_us(time_since_last_pen_up, now)};
      if (timediff < DOUBLE_TAP_PERIOD_THRESHOLD_US &&
          // timediff > DOUBLE_TAP_PERIOD_THRESHOLD_US / 2 &&
          std::abs(touch_since_last_pen_up.x - prv_touch.x) < 30 &&
          std::abs(touch_since_last_pen_up.y - prv_touch.y) < 30) {
        fill_routine(emerald);
      } else {
        // undo_cool_touch_action(touch_to_undo);
        take_cool_touch_action(prv_touch);
      }
      time_since_last_pen_up = now;
      touch_since_last_pen_up = prv_touch;
      state = TouchMachine::WAIT;
      // touch_to_undo = prv_touch;
    }
    sleep_ms(TOUCH_POLL_INTERVAL_MS);
  }
}

} // namespace demo