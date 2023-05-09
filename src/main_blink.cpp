extern "C"
{
#include "../driver/dispWaveshareLcd.h"
}
#include "pico/stdlib.h"
#include "status_utilities.hpp"

static uint8_t buffer[DISP_WIDTH * DISP_HEIGHT]{};

void clear(uint8_t *buf, size_t len)
{
  for (size_t ii = 0; ii < len; ++ii)
  {
    buf[ii] = 0;
  }
}

int main()
{

  stdio_init_all();

  clear(buffer, DISP_WIDTH * DISP_HEIGHT);

  sleep_ms(5000);
  const auto okay{dispInit(buffer, 1)};

  if (okay)
  {
    BlinkStatus{BlinkStatus::Milliseconds{1000}}.blink_forever();
  }
  else
  {
    BlinkStatus{BlinkStatus::Milliseconds{250}}.blink_forever();
  }
}