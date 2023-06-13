#include "../driver/dispWaveshareLcd.h"
#include <algorithm>
#include <array>
#include <limits>

#include "pico/stdlib.h"
#include "pico/printf.h"
#include "pico/multicore.h"

#include "status_utilities.hpp"
#include "../driver/pinout.h"
#include "TextOut.hpp"
#include "VideoBuf.hpp"

#include "tusb.h"

static constexpr uint8_t BPP{1};
static constexpr size_t BUFLEN{[]
                               {
                                 // pixels per byte is a function of bpp
                                 // need to convert from number of pixels to number of bits, then to number of bytes.
                                 return DISP_WIDTH * DISP_HEIGHT * BPP / 8;
                               }()};

constexpr void init_to_all_white(auto &buf)
{
  for (auto &el : buf)
  {
    el = std::numeric_limits<uint8_t>::min();
  }
}
constexpr void init_to_all_black(auto &buf)
{
  for (auto &el : buf)
  {
    el = std::numeric_limits<uint8_t>::max();
  }
}

constexpr std::array<uint8_t, BUFLEN> init_the_buffer()
{
  std::array<uint8_t, BUFLEN> rv{};
  init_to_all_black(rv);

  return rv;
}

static auto buffer{init_the_buffer()};
static TileBuffer<DISP_WIDTH, DISP_HEIGHT, BPP> tile_buf{buffer};
static TextOut wrt{tile_buf};

void setup_for_input(uint id)
{
  gpio_init(id);
  gpio_set_dir(id, false);
}
void setup_for_output(uint id)
{
  gpio_init(id);
  gpio_set_dir(id, true);
}

bool lcd_init(auto &video_buf)
{
  bool status{true};
  setup_for_output(PIN_TOUCH_CS);
  setup_for_output(PIN_LCD_RESET);
  setup_for_output(PIN_LCD_DnC);
  setup_for_output(PIN_LCD_CS);
  setup_for_output(PIN_SPI_CLK);
  setup_for_output(PIN_SPI_MOSI);
  setup_for_input(PIN_SPI_MISO);
  setup_for_output(PIN_LCD_BL);

  status &= dispInit(std::data(video_buf), 1);
  dispSetDepth(BPP);

  printf("INFO: Turning on backlight\n");

  gpio_put(PIN_LCD_BL, true);

  return status;
}
int main()
{
  static_assert(BPP == 1, "init_letter_list(): Haven't handled more that 1 bit per pixel yet. Sorry.");
  stdio_init_all();

  tusb_init();

  if (!lcd_init(buffer))
  {
    BlinkStatus{BlinkStatus::Milliseconds{250}}.blink_forever();
  }

  // That's right, this is its only job.
  multicore_launch_core1([]()
                         { BlinkStatus{BlinkStatus::Milliseconds{1000}}.blink_forever(); });

  clear(wrt);
  print(wrt, "+------------+\n");
  print(wrt, "| Meven 2040 |\n");
  print(wrt, "+------------+\n");
  sleep_ms(1000);
  for (;;)
  {
    // tuh_task();// I guess this is required?

    // run an animation, by hand
    print(wrt, "C:\\> ");
    sleep_ms(500);
    static constexpr std::string_view the_stuff{R"(This is a story all about how my life got switched turned upside down so take a minute just sit right there while I tell you howibecametheprince of a town called BelAir.)"};
    for (const auto c : the_stuff)
    {
      print(wrt, c);
      sleep_ms(50);
    }
    sleep_ms(500);
    print(wrt, '\n');
    sleep_ms(100);
    print(wrt, "Bad batch file or command\n");
    sleep_ms(200);
  }
}