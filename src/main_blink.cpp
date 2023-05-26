#include "../driver/dispWaveshareLcd.h"
#include <algorithm>
#include <array>
#include <limits>
#include "pico/stdlib.h"
#include "pico/rand.h"
#include "pico/printf.h"
#include "status_utilities.hpp"
#include "../driver/pinout.h"
#include "../glyphs/letters.hpp"
// #include "../glyphs/print.hpp"
#include "TextOut.hpp"

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

  TextOut<DISP_WIDTH, DISP_HEIGHT, BPP, glyphs::LetterType> wrt{rv};
  print(wrt, "      \n\tMy\nMeven 2040\n");
  print(wrt, "\n");
  print(wrt, "abcdefghijklmnopqrstuvwxyz\n");
  print(wrt, "ABCDEFGHIJKLMNOPQRSTUVWXYZ\n");
  print(wrt, "0123456789\n");
  print(wrt, "\n");
  print(wrt, "!@#$%^&*()-_=+{}[]|\n");
  print(wrt, "\\/~`:;'\"<>,.?\n");
  print(wrt, "\n");
  print(wrt, "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz\n");

  return rv;
}

static auto buffer{init_the_buffer()};

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
  stdio_init_all();

  if (!lcd_init(buffer))
  {
    BlinkStatus{BlinkStatus::Milliseconds{250}}.blink_forever();
  }

  // TODO: kick this off on the other core.  That's right, this will be its only job.
  BlinkStatus{BlinkStatus::Milliseconds{1000}}.blink_forever();

  // static constexpr std::array<LetterType, 26 * 2> dictionary{};
  // TextOut text_display(buffer, dictionary, DISP_WIDTH / 8, DISP_HEIGHT);

  // print(text_display, "This is a string\n");
  // draw(text_display, "This is another string!", 0, 10);

  // AbstractOut abstract_display(buffer, dictionary, DISP_WIDTH / 8, DISP_HEIGHT);
}