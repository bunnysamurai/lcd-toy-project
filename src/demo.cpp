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

#include "gamepad/gamepad.hpp"
#include "screen/TileDef.h"
#include "screen/constexpr_tile_utils.hpp"
#include "screen/gfx/shapes.hpp"
#include "screen/screen.hpp"

#include "common/pico_sdk_timer_details.hpp"
#include "common/timer.hpp"
#include "common/utilities.hpp"

namespace demo {

void fill_routine(const screen::Tile &tile) noexcept {
  const auto dims{screen::get_virtual_screen_size()};
  for (size_t yy = 0; yy < dims.height; yy += tile.side_length) {
    for (size_t xx = 0; xx < dims.width; xx += tile.side_length) {
      screen::draw_tile(xx, yy, tile);
    }
  }
}

static constexpr std::array Demo_Palette{
    /* clang-format off */
  screen::Clut{.r = 0, .g = 0, .b = 0}, /* Black */
  screen::Clut{.r = 255, .g = 0, .b = 255}, /* Magenta */
  screen::Clut{.r = 255, .g = 255, .b = 0 }, /* Yellow */
  screen::Clut{.r = 0, .g = 255, .b = 255}, /* Cyan */
  screen::Clut{.r = 255, .g = 0, .b = 0}, /* Red */
  screen::Clut{.r = 0, .g = 255, .b = 0}, /* Green */
  screen::Clut{.r = 0, .g = 0, .b = 255}, /* Blue */
    /* clang-format on */
};
static constexpr uint32_t BLACK{0};
static constexpr uint32_t MAGENTA{1};
static constexpr uint32_t YELLOW{2};
static constexpr uint32_t BLUE{3};
static constexpr uint32_t RED{4};
static constexpr uint32_t GREEN{5};
static constexpr uint32_t CYAN{6};

/* I think 8x8 tiles are okay? */
static constexpr auto magenta_data{embp::pfold(
    MAGENTA, MAGENTA, MAGENTA, MAGENTA, MAGENTA, MAGENTA, MAGENTA, MAGENTA,
    MAGENTA, MAGENTA, MAGENTA, MAGENTA, MAGENTA, MAGENTA, MAGENTA, MAGENTA,
    MAGENTA, MAGENTA, MAGENTA, MAGENTA, MAGENTA, MAGENTA, MAGENTA, MAGENTA,
    MAGENTA, MAGENTA, MAGENTA, MAGENTA, MAGENTA, MAGENTA, MAGENTA, MAGENTA,
    MAGENTA, MAGENTA, MAGENTA, MAGENTA, MAGENTA, MAGENTA, MAGENTA, MAGENTA,
    MAGENTA, MAGENTA, MAGENTA, MAGENTA, MAGENTA, MAGENTA, MAGENTA, MAGENTA,
    MAGENTA, MAGENTA, MAGENTA, MAGENTA, MAGENTA, MAGENTA, MAGENTA, MAGENTA,
    MAGENTA, MAGENTA, MAGENTA, MAGENTA, MAGENTA, MAGENTA, MAGENTA, MAGENTA)};
static constexpr auto gold_data{embp::pfold(
    YELLOW, YELLOW, YELLOW, YELLOW, YELLOW, YELLOW, YELLOW, YELLOW, YELLOW,
    YELLOW, YELLOW, YELLOW, YELLOW, YELLOW, YELLOW, YELLOW, YELLOW, YELLOW,
    YELLOW, YELLOW, YELLOW, YELLOW, YELLOW, YELLOW, YELLOW, YELLOW, YELLOW,
    YELLOW, YELLOW, YELLOW, YELLOW, YELLOW, YELLOW, YELLOW, YELLOW, YELLOW,
    YELLOW, YELLOW, YELLOW, YELLOW, YELLOW, YELLOW, YELLOW, YELLOW, YELLOW,
    YELLOW, YELLOW, YELLOW, YELLOW, YELLOW, YELLOW, YELLOW, YELLOW, YELLOW,
    YELLOW, YELLOW, YELLOW, YELLOW, YELLOW, YELLOW, YELLOW, YELLOW, YELLOW,
    YELLOW)};
static constexpr auto emerald_data{embp::pfold(
    CYAN, CYAN, CYAN, CYAN, CYAN, CYAN, CYAN, CYAN, CYAN, CYAN, CYAN, CYAN,
    CYAN, CYAN, CYAN, CYAN, CYAN, CYAN, CYAN, CYAN, CYAN, CYAN, CYAN, CYAN,
    CYAN, CYAN, CYAN, CYAN, CYAN, CYAN, CYAN, CYAN, CYAN, CYAN, CYAN, CYAN,
    CYAN, CYAN, CYAN, CYAN, CYAN, CYAN, CYAN, CYAN, CYAN, CYAN, CYAN, CYAN,
    CYAN, CYAN, CYAN, CYAN, CYAN, CYAN, CYAN, CYAN, CYAN, CYAN, CYAN, CYAN,
    CYAN, CYAN, CYAN, CYAN)};
static constexpr auto red_data{
    embp::pfold(RED, RED, RED, RED, RED, RED, RED, RED, RED, RED, RED, RED, RED,
                RED, RED, RED, RED, RED, RED, RED, RED, RED, RED, RED, RED, RED,
                RED, RED, RED, RED, RED, RED, RED, RED, RED, RED, RED, RED, RED,
                RED, RED, RED, RED, RED, RED, RED, RED, RED, RED, RED, RED, RED,
                RED, RED, RED, RED, RED, RED, RED, RED, RED, RED, RED, RED)};
static constexpr auto green_data{embp::pfold(
    GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN,
    GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN,
    GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN,
    GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN,
    GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN,
    GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN)};
static constexpr auto blue_data{embp::pfold(
    BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE,
    BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE,
    BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE,
    BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE,
    BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE,
    BLUE, BLUE, BLUE, BLUE)};

constexpr screen::Tile blue{.side_length = 8,
                            .format = screen::Format::RGB565_LUT4,
                            .data = std::data(blue_data)};
constexpr screen::Tile green{.side_length = 8,
                             .format = screen::Format::RGB565_LUT4,
                             .data = std::data(green_data)};
constexpr screen::Tile red{.side_length = 8,
                           .format = screen::Format::RGB565_LUT4,
                           .data = std::data(red_data)};
constexpr screen::Tile magenta{.side_length = 8,
                               .format = screen::Format::RGB565_LUT4,
                               .data = std::data(magenta_data)};
constexpr screen::Tile gold{.side_length = 8,
                            .format = screen::Format::RGB565_LUT4,
                            .data = std::data(gold_data)};
constexpr screen::Tile emerald{.side_length = 8,
                               .format = screen::Format::RGB565_LUT4,
                               .data = std::data(emerald_data)};

void run_color_rando_art() noexcept {
  gamepad::five::init();

  static constexpr std::array<screen::Tile, 4> tilelist{gold, green, gold,
                                                        gold};
  const auto dims{screen::get_virtual_screen_size()};

  auto &&random_routine{[&](uint32_t rando_select) {
    if (rando_select == 0) {
      for (size_t yy = 0; yy < (dims.height >> 1);
           yy += tilelist[0].side_length) {
        for (size_t xx = 0; xx < (dims.width >> 1);
             xx += tilelist[0].side_length) {
          const auto r{get_rand_32() & 0b11};
          const auto &rtile{tilelist[r]};
          screen::draw_tile(xx, yy, rtile);
          screen::draw_tile(dims.width - xx - rtile.side_length, yy, rtile);
          screen::draw_tile(xx, dims.height - yy - rtile.side_length, rtile);
          screen::draw_tile(dims.width - xx - rtile.side_length,
                            dims.height - yy - rtile.side_length, rtile);
        }
      }
    } else if (rando_select == 1) {
      const auto r{get_rand_32() & 0b1111};
      const auto color{(r & 0b11) + 1};
      screen::fill_screen(CYAN);

      for (uint32_t ii = 50; ii < 100; ii += 7) {
        screen::gfx::draw_line({.x = ii, .y = ii}, {.x = ii + 50, .y = ii},
                               color, 1 + (r >> 2));
        screen::gfx::draw_line({.x = ii, .y = ii}, {.x = ii, .y = ii + 50},
                               color, 1 + (r >> 2));
      }

      /* test all 8 octets */
      screen::gfx::draw_line({.x = 100, .y = 100}, {.x = 175, .y = 110}, color,
                             1);
      screen::gfx::draw_line({.x = 100, .y = 100}, {.x = 175, .y = 90}, color,
                             1);
      screen::gfx::draw_line({.x = 100, .y = 100}, {.x = 110, .y = 175}, color,
                             1);
      screen::gfx::draw_line({.x = 100, .y = 100}, {.x = 110, .y = 25}, color,
                             1);

      screen::gfx::draw_line({.x = 100, .y = 100}, {.x = 25, .y = 110}, color,
                             1);
      screen::gfx::draw_line({.x = 100, .y = 100}, {.x = 25, .y = 90}, color,
                             1);
      screen::gfx::draw_line({.x = 100, .y = 100}, {.x = 90, .y = 175}, color,
                             1);
      screen::gfx::draw_line({.x = 100, .y = 100}, {.x = 90, .y = 25}, color,
                             1);
    }
  }};

  screen::clear_screen();
  screen::set_format(screen::Format::RGB565_LUT4);
  screen::init_clut(std::data(Demo_Palette), std::size(Demo_Palette));
  screen::fill_screen(CYAN | (CYAN << 4));

  absolute_time_t prev_time{get_absolute_time()};
  uint32_t selection_idx{1};
  random_routine(selection_idx);
  while (true) {
    const auto now{get_absolute_time()};
    if (absolute_time_diff_us(prev_time, now) > 2000000) {
      random_routine(selection_idx);
      prev_time += 2000000;
    }
    sleep_ms(1);
    const auto state{gamepad::five::get()};
    if (state.etc) {
      break;
    }
    if (state.left || state.right) {
      selection_idx = (selection_idx + 1) & 0b1;
      screen::fill_screen(CYAN | (CYAN << 4));
    }
  }

  screen::melt(BLACK);

  gamepad::five::deinit();
}

struct Velocity {
  int32_t dx;
  int32_t dy;
};
struct Position {
  uint32_t x;
  uint32_t y;
};
void run_linebounce_screensaver() noexcept {
  const auto dim{screen::get_virtual_screen_size()};

  /* we'll use S18.14 format for the position and velocity */
  std::array<Position, 4> linep{
      Position{.x = 10 << 14, .y = 11 << 14},
      Position{.x = 20 << 14, .y = 300 << 14},
      Position{.x = 200 << 14, .y = 300 << 14},
      Position{.x = 200 << 14, .y = 30 << 14},
  };

  /* in pixels per update tick */
  std::array<Velocity, 4> linev{
      Velocity{.dx = 10000, .dy = 11000},
      Velocity{.dx = -10000, .dy = -11000},
      Velocity{.dx = 10000, .dy = 11000},
      Velocity{.dx = -10000, .dy = -11000},
  };

  screen::clear_screen();
  screen::set_format(screen::Format::RGB565_LUT4);
  screen::init_clut(std::data(Demo_Palette), std::size(Demo_Palette));
  screen::fill_screen(BLACK | (BLACK << 4));

  Timer<timer_details::PicoSdk> button_timer{1000};
  /* timer kicks every 1/60 seconds. */
  Timer<timer_details::PicoSdk> update_timer{1 << 14};
  std::array<screen::gfx::Point, 4> prvpoints{};
  std::array<screen::gfx::Point, 4> points{};
  auto color{RED};
  for (;;) {
    if (update_timer.elapsed()) {
      update_timer.reset();

      for (int ii = 0; ii < std::size(linep); ++ii) {
        linep[ii].x += linev[ii].dx;
        if ((linep[ii].x >> 14) >= dim.width) {
          linep[ii].x = linev[ii].dx < 0 ? 0 : (dim.width - 1) << 14;
          linev[ii].dx = -linev[ii].dx;
          const auto r{get_rand_32() & 0b11};
          color = r + 1;
        }
        linep[ii].y += linev[ii].dy;
        if ((linep[ii].y >> 14) >= dim.height) {
          linep[ii].y = linev[ii].dy < 0 ? 0 : (dim.height - 1) << 14;
          linev[ii].dy = -linev[ii].dy;
          const auto r{get_rand_32() & 0b11};
          color = r + 1;
        }
        points[ii].x = linep[ii].x >> 14;
        points[ii].y = linep[ii].y >> 14;
      }

      screen::gfx::draw_line(prvpoints[0], prvpoints[1], BLACK, 1);
      screen::gfx::draw_line(points[0], points[1], color, 1);
      screen::gfx::draw_line(prvpoints[1], prvpoints[2], BLACK, 1);
      screen::gfx::draw_line(points[1], points[2], color, 1);
      screen::gfx::draw_line(prvpoints[2], prvpoints[3], BLACK, 1);
      screen::gfx::draw_line(points[2], points[3], color, 1);
      screen::gfx::draw_line(prvpoints[3], prvpoints[0], BLACK, 1);
      screen::gfx::draw_line(points[3], points[0], color, 1);
      prvpoints[0] = points[0];
      prvpoints[1] = points[1];
      prvpoints[2] = points[2];
      prvpoints[3] = points[3];
    }

    if (button_timer.elapsed()) {
      button_timer.reset();
      const auto state{gamepad::five::get()};
      if (state.etc) {
        break;
      }
    }
  }
}

void run_text_animation() noexcept {
  if (!screen::set_console_mode()) {
    return;
  }
  printf("+------------------+\n");
  printf("| Meven 2040 Demo! |\n");
  printf("+------------------+\n");
  sleep_ms(1000);
  while (true) {
    // run an animation, by hand
    printf("C:\\> ");
    // sleep_ms(500);
    sleep_ms(100);
    static constexpr std::string_view the_stuff{
        R"(This is a story all about how my life got switched turned upside down so take a minute just sit right there while I tell you howibecametheprince of a town called BelAir.)"};
    for (const auto c : the_stuff) {
      printf("%c", c);
      sleep_ms(10);
    }
    // sleep_ms(500);
    sleep_ms(100);
    printf("\n");
    // sleep_ms(100);
    sleep_ms(20);
    printf("Bad batch file or command\n");
    // sleep_ms(200);
    sleep_ms(50);
  }
}

enum struct TouchMachine { WAIT, PEN_DOWN, PEN_UP };

namespace {

void initialize_cool_touch_demo() noexcept {
  screen::clear_screen();
  screen::set_format(screen::Format::RGB565_LUT4);
  screen::init_clut(std::data(Demo_Palette), std::size(Demo_Palette));
  fill_routine(emerald);
  sleep_ms(1000000);
}

[[nodiscard]] screen::TouchReport
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
                             .pen_up = rawloc.pen_up};
}

void undo_cool_touch_action(screen::TouchReport touch_loc) noexcept {
  if (touch_loc.x < 0 || touch_loc.y < 0) {
    return;
  }
  touch_loc = to_pixelspace(touch_loc);
  const auto dims{screen::get_virtual_screen_size()};
  screen::draw_tile(dims.width - touch_loc.x - 1, dims.height - touch_loc.y - 1,
                    emerald);
}

void take_cool_touch_action(screen::TouchReport touch_loc) noexcept {
  touch_loc = to_pixelspace(touch_loc);
  const auto dims{screen::get_virtual_screen_size()};
  screen::draw_tile(dims.width - touch_loc.x - 1, dims.height - touch_loc.y - 1,
                    red);
}

} // namespace

static constexpr int64_t DOUBLE_TAP_PERIOD_THRESHOLD_US{550 *
                                                        1000}; /* 100 ms? */
void run_touch_demo(TouchConfig cfg) noexcept {
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