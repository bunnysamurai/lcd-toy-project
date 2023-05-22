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
#include "../glyphs/print.hpp"

static constexpr uint8_t BPP{1};
static constexpr size_t BUFLEN{[]
                               {
                                 // pixels per byte is a function of bpp
                                 // need to convert from number of pixels to number of bits, then to number of bytes.
                                 return DISP_WIDTH * DISP_HEIGHT * BPP / 8;
                               }()};
static std::array<uint8_t, BUFLEN> buffer;

void init_to_all_white(auto &buf)
{
  std::fill(std::begin(buffer), std::end(buffer), std::numeric_limits<uint8_t>::min());
}
void init_to_all_black(auto &buf)
{
  std::fill(std::begin(buffer), std::end(buffer), std::numeric_limits<uint8_t>::max());
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
int main()
{
  stdio_init_all();

  init_to_all_black(buffer);
  glyphs::raw_print(buffer, "STEVEN\n\tMY\nMEVEN 2040", 0, 0);

  glyphs::raw_print(buffer, "abcdefghijklmnopqrstuvwxyz", 0, 4 * 8);
  glyphs::raw_print(buffer, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", 0, 5 * 8);
  glyphs::raw_print(buffer, "0123456789", 0, 6 * 8);
  glyphs::raw_print(buffer, "!@#$%^&*()-_=+{}[]|", 0, 7 * 8);
  glyphs::raw_print(buffer, "\\/~`:;'\"<>,.?", 0, 8 * 8);

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