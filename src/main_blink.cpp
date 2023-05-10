#include "../driver/dispWaveshareLcd.h"
#include <algorithm>
#include <array>
#include <limits>
#include "pico/stdlib.h"
#include "pico/printf.h"
#include "status_utilities.hpp"

static constexpr uint8_t BPP{1};
static constexpr size_t BUFLEN{[]
                               {
                                 // pixels per byte is a function of bpp
                                 // need to convert from number of pixels to number of bits, then to number of bytes.
                                 return DISP_WIDTH * DISP_HEIGHT * BPP / 8;
                               }()};
static std::array<uint8_t, BUFLEN> buffer;

int main()
{
  stdio_init_all();

  std::fill(std::begin(buffer), std::end(buffer), std::numeric_limits<uint8_t>::max());

  sleep_ms(5000);

  printf("INFO: First element in buffer is %d\n", buffer[0]);

  if (!dispInit(std::data(buffer), 1))
  {
    BlinkStatus{BlinkStatus::Milliseconds{250}}.blink_forever();
  }

  dispSetDepth(BPP);

  BlinkStatus{BlinkStatus::Milliseconds{1000}}.blink_forever();
}