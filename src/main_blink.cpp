#include "../driver/dispWaveshareLcd.h"
#include <algorithm>
#include <array>
#include <limits>
#include "pico/stdlib.h"
#include "pico/printf.h"
#include "status_utilities.hpp"
#include "../driver/pinout.h"

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

using LetterType = std::array<uint8_t, 8>;

/**
 * @param x Column, in characters
 * @param y Row, in characters
 */
void write_letter(auto &video_buf, uint x, uint y, const LetterType &letter)
{
  // characters are 8x8 pixels in size
  // and we need to index by character
  // x positions are byte indexes in the video buffer
  // y positions are 8row increments
  static constexpr auto LCD_EFFECTIVE_WIDTH{DISP_WIDTH / 8};
  for (uint idx = y * LCD_EFFECTIVE_WIDTH + x, ii = 0; ii < letter.size(); idx += LCD_EFFECTIVE_WIDTH, ++ii)
  {
    video_buf[idx] = letter[ii];
  }
}

inline constexpr uint8_t reverse_bits(uint8_t val)
{
  uint8_t out{};
  for (uint ii = 0; ii < 7; ++ii)
  {
    out = (out + ((val >> ii) & 0x01)) << 1;
  }
  out = (out + ((val >> 7) & 0x01));
  return out;
}
inline constexpr LetterType reverse(LetterType letter)
{
  for (auto &c : letter)
  {
    c = reverse_bits(c);
  }
  return letter;
}
inline constexpr LetterType invert(LetterType letter)
{
  for (auto &c : letter)
  {
    c = ~c;
  }
  return letter;
}

// Let's do 8x8 row, cols
inline constexpr LetterType letter_N()
{
  return {
      0b100001'00,
      0b110001'00,
      0b101001'00,
      0b101001'00,
      0b100101'00,
      0b100011'00,
      0b100001'00};
}
inline constexpr LetterType letter_V()
{
  return {
      0b100001'00,
      0b100001'00,
      0b100001'00,
      0b010010'00,
      0b010010'00,
      0b010010'00,
      0b001100'00};
}
inline constexpr LetterType letter_E()
{
  return {
      0b111111'00,
      0b100000'00,
      0b100000'00,
      0b111110'00,
      0b100000'00,
      0b100000'00,
      0b111111'00};
}
inline constexpr LetterType letter_T()
{
  return {
      0b011111'00,
      0b000100'00,
      0b000100'00,
      0b000100'00,
      0b000100'00,
      0b000100'00,
      0b000100'00};
}
inline constexpr LetterType letter_S()
{
  return {
      0b001110'00,
      0b010001'00,
      0b100000'00,
      0b011110'00,
      0b000001'00,
      0b100001'00,
      0b011110'00};
}
inline constexpr LetterType letter_A()
{
  return {
      0b001100'00,
      0b010010'00,
      0b010010'00,
      0b011110'00,
      0b100001'00,
      0b100001'00,
      0b100001'00};
}

static constexpr LetterType CAPITAL_S{reverse(invert(letter_S()))};
static constexpr LetterType CAPITAL_T{reverse(invert(letter_T()))};
static constexpr LetterType CAPITAL_E{reverse(invert(letter_E()))};
static constexpr LetterType CAPITAL_V{reverse(invert(letter_V()))};
static constexpr LetterType CAPITAL_N{reverse(invert(letter_N()))};

void spell_steven(auto &video_buf, uint x, uint y)
{
  write_letter(video_buf, x++, y, CAPITAL_S);
  write_letter(video_buf, x++, y, CAPITAL_T);
  write_letter(video_buf, x++, y, CAPITAL_E);
  write_letter(video_buf, x++, y, CAPITAL_V);
  write_letter(video_buf, x++, y, CAPITAL_E);
  write_letter(video_buf, x++, y, CAPITAL_N);
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

int main()
{
  stdio_init_all();

  setup_for_output(PIN_TOUCH_CS);
  setup_for_output(PIN_LCD_RESET);
  setup_for_output(PIN_LCD_DnC);
  setup_for_output(PIN_LCD_CS);
  setup_for_output(PIN_SPI_CLK);
  setup_for_output(PIN_SPI_MOSI);
  setup_for_input(PIN_SPI_MISO);
  setup_for_output(PIN_LCD_BL);

  init_to_all_black(buffer);
  spell_steven(buffer, 0, 0);

  printf("INFO: First element in buffer is %d\n", buffer[0]);

  if (!dispInit(std::data(buffer), 1))
  {
    BlinkStatus{BlinkStatus::Milliseconds{250}}.blink_forever();
  }

  dispSetDepth(BPP);

  printf("INFO: Turning on backlight\n");
  gpio_put(PIN_LCD_BL, true);

  BlinkStatus{BlinkStatus::Milliseconds{1000}}.blink_forever();
}