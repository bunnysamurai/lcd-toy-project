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

/**
 * @param x Column, in characters
 * @param y Row, in characters
 */
void write_letter(auto &video_buf, uint x, uint y, const glyphs::LetterType &letter)
{
  // characters are 8x8 pixels in size
  // and we need to index by character
  // x positions are byte indexes in the video buffer
  // y positions are 8row increments
  static constexpr auto LCD_EFFECTIVE_WIDTH{DISP_WIDTH / 8};
  for (uint idx = y * LCD_EFFECTIVE_WIDTH + x, ii = 0; ii < size(letter); idx += LCD_EFFECTIVE_WIDTH, ++ii)
  {
    video_buf[idx] = letter[ii];
  }
}

void erase_steven(auto &video_buf, uint x, uint y)
{
  write_letter(video_buf, x++, y, glyphs::SPACE);
  write_letter(video_buf, x++, y, glyphs::SPACE);
  write_letter(video_buf, x++, y, glyphs::SPACE);
  write_letter(video_buf, x++, y, glyphs::SPACE);
  write_letter(video_buf, x++, y, glyphs::SPACE);
  write_letter(video_buf, x++, y, glyphs::SPACE);
}
void erase_my(auto &video_buf, uint x, uint y)
{
  write_letter(video_buf, x++, y, glyphs::SPACE);
  write_letter(video_buf, x++, y, glyphs::SPACE);
}
void erase_meven(auto &video_buf, uint x, uint y)
{
  write_letter(video_buf, x++, y, glyphs::SPACE);
  write_letter(video_buf, x++, y, glyphs::SPACE);
  write_letter(video_buf, x++, y, glyphs::SPACE);
  write_letter(video_buf, x++, y, glyphs::SPACE);
  write_letter(video_buf, x++, y, glyphs::SPACE);
}

void spell_2040(auto &video_buf, uint x, uint y)
{
  write_letter(video_buf, x++, y, glyphs::NUM_2);
  write_letter(video_buf, x++, y, glyphs::NUM_0);
  write_letter(video_buf, x++, y, glyphs::NUM_4);
  write_letter(video_buf, x++, y, glyphs::NUM_0);
}
void spell_my(auto &video_buf, uint x, uint y)
{
  write_letter(video_buf, x++, y, glyphs::CAPITAL_M);
  write_letter(video_buf, x++, y, glyphs::CAPITAL_Y);
}
void spell_steven(auto &video_buf, uint x, uint y)
{
  write_letter(video_buf, x++, y, glyphs::CAPITAL_S);
  write_letter(video_buf, x++, y, glyphs::CAPITAL_T);
  write_letter(video_buf, x++, y, glyphs::CAPITAL_E);
  write_letter(video_buf, x++, y, glyphs::CAPITAL_V);
  write_letter(video_buf, x++, y, glyphs::CAPITAL_E);
  write_letter(video_buf, x++, y, glyphs::CAPITAL_N);
}
void spell_meven(auto &video_buf, uint x, uint y)
{
  write_letter(video_buf, x++, y, glyphs::CAPITAL_M);
  write_letter(video_buf, x++, y, glyphs::CAPITAL_E);
  write_letter(video_buf, x++, y, glyphs::CAPITAL_V);
  write_letter(video_buf, x++, y, glyphs::CAPITAL_E);
  write_letter(video_buf, x++, y, glyphs::CAPITAL_N);
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
  const uint x{get_rand_32() % DISP_WIDTH / 8};
  const uint y{get_rand_32() * 8 % DISP_HEIGHT};
  spell_meven(buffer, x, y + 0 * 8);
  spell_2040(buffer, x, y + 1 * 8);

  printf("INFO: First element in buffer is %d\n", buffer[0]);

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