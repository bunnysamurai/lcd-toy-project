#include "demo.hpp"

#include <array>
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
  static constexpr std::array<Tile, 4> tilelist{gold, green, green, gold};
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
  auto &&fill_routine{[&](const Tile &tile) {
    for (size_t yy = 0; yy < dims.height; yy += tile.side_length) {
      for (size_t xx = 0; xx < dims.width; xx += tile.side_length) {
        bsio::draw_tile(xx, yy, tile);
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

void run_touch_demo() {
  /* sample about every 10 ms?
   * we should really setup a timer for this... */
  static constexpr auto TOUCH_POLL_INTERVAL_MS{10};

  screen::TouchReport touch;
  screen::TouchReport prv_touch;
  TouchMachine state{TouchMachine::WAIT};
  while (true) {
    switch (state) {
    case TouchMachine::WAIT:
      if (screen::get_touch_report(touch) && !touch.pen_up) {
        bsio::clear_console();
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
        }
      }
      break;
    case TouchMachine::PEN_UP:
      printf("Touch ended at {%d, %d}\n", prv_touch.x, prv_touch.y);
      state = TouchMachine::WAIT;
    }
    sleep_ms(TOUCH_POLL_INTERVAL_MS);
  }
}

} // namespace demo