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

void run_demo_animation(auto stop_looping_callback)
{
  auto &&quit_otherwise_sleep{[&](uint interval_ms)
                              {
                                if (stop_looping_callback())
                                {
                                  return true;
                                }
                                sleep_ms(interval_ms);
                                return false;
                              }};
  clear(wrt);
  print(wrt, "+------------------+\n");
  print(wrt, "| Meven 2040 Demo! |\n");
  print(wrt, "+------------------+\n");
  if (quit_otherwise_sleep(1000))
  {
    return;
  }
  while (!stop_looping_callback())
  {
    // run an animation, by hand
    print(wrt, "C:\\> ");
    if (quit_otherwise_sleep(500))
    {
      return;
    }
    static constexpr std::string_view the_stuff{R"(This is a story all about how my life got switched turned upside down so take a minute just sit right there while I tell you howibecametheprince of a town called BelAir.)"};
    bool please_quit{false};
    for (const auto c : the_stuff)
    {
      print(wrt, c);
      if (quit_otherwise_sleep(50))
      {
        return;
      }
    }
    if (quit_otherwise_sleep(500))
    {
      return;
    }
    print(wrt, '\n');
    if (quit_otherwise_sleep(100))
    {
      return;
    }
    print(wrt, "Bad batch file or command\n");
    if (quit_otherwise_sleep(200))
    {
      return;
    }
  }
}
void handle_putchar(uint8_t c)
{
  putc(wrt, static_cast<char>(c));
  // TODO hack this in here for now
  if (c == '\n')
  {
    print(wrt, "[meven]$ ");
  }
}
void handle_print(const char *str)
{
  print(wrt, str);
  // TODO hack this in here for now
  if (str[strlen(str) - 1] == '\n')
  {
    print(wrt, "[meven]$ ");
  }
}

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

bool stop_demo{false}; // TODO globals are everywhere, be definition ;)
void set_stop_demo()
{
  // FIXME race conditions
  stop_demo = true;
}

int main()
{
  static_assert(BPP == 1, "init_letter_list(): Haven't handled more that 1 bit per pixel yet. Sorry.");

  tusb_init();

  if (!lcd_init(buffer))
  {
    BlinkStatus{BlinkStatus::Milliseconds{250}}.blink_forever();
  }

  // That's right, this is its only job.
  // multicore_launch_core1([]()
  //                        { BlinkStatus{BlinkStatus::Milliseconds{1000}}.blink_forever(); });

  // start by running the demo
  // FIXME race conditions
  multicore_launch_core1([]
                         { run_demo_animation([&]()
                                              { return stop_demo; }); });
  while (!stop_demo)
  {
    tuh_task(); // I guess this is required?  Looks to handle all pending interrupts, then returns
  }
  sleep_ms(1000); // TODO hack sleep to wait for the animation to stop running.  There's probably a way to check if the other core is halted...
  clear(wrt);
  print(wrt, "[meven]$ ");
  for (;;)
  {
    tuh_task(); // I guess this is required?  Looks to handle all pending interrupts, then returns

    // all processing handed in the keyboard processing callback. See hid_app.cpp for more details.
  }
}