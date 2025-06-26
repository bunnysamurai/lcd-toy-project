#include "../basic_io/screen/screen.hpp"
#include <algorithm>
#include <array>
#include <limits>

#include "pico/stdlib.h"
#include "pico/printf.h"
#include "pico/multicore.h"

#include "status_utilities.hpp"
#include "TextOut.hpp"
// #include "TextConsole.hpp"
#include "../glyphs/letters.hpp"
#include "VideoBuf.hpp"

#include "../basic_io/keyboard/TinyUsbKeyboard.hpp"

static constexpr uint8_t BPP{1};
static constexpr uint32_t DISPLAY_WIDTH{screen::PHYSICAL_SIZE.width};
static constexpr uint32_t DISPLAY_HEIGHT{screen::PHYSICAL_SIZE.height};
static constexpr size_t BUFLEN{[]
                               {
                                 // pixels per byte is a function of bpp
                                 // need to convert from number of pixels to number of bits, then to number of bytes.
                                 return DISPLAY_WIDTH * DISPLAY_HEIGHT * BPP / 8;
                               }()};

template <class T, class U>
constexpr void init_to_all_val(T &buf, const U &val)
{
  for (auto &el : buf)
  {
    el = val;
  }
}

constexpr std::array<uint8_t, BUFLEN> init_the_buffer()
{
  std::array<uint8_t, BUFLEN> rv{};
  init_to_all_val(rv, std::numeric_limits<uint8_t>::min());

  return rv;
}

static auto raw_video_buffer{init_the_buffer()};

static bool screen_init()
{
  return screen::init(std::data(raw_video_buffer), {.row = 0, .column = 0}, {.width = DISPLAY_WIDTH, .height = DISPLAY_HEIGHT}, {.bpp = BPP});
}

static bool screen_io_init()
{

}

int main()
{
  static_assert(BPP == 1, "init_letter_list(): Haven't handled more that 1 bit per pixel yet. Sorry.");

  if (!screen_init())
  {
    BlinkStatus{BlinkStatus::Milliseconds{250}}.blink_forever();
  }

  if (!screen_io_init())
  {

  }
}