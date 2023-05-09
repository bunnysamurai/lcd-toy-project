extern "C" {
#include "../driver/dispWaveshareLcd.h"
}
#include "pico/stdlib.h"

static uint8_t buffer[DISP_WIDTH * DISP_HEIGHT]{};

void clear(uint8_t *buf, size_t len) {
  for (size_t ii = 0; ii < len; ++ii) {
    buf[ii] = 0;
  }
}

int main() {

  static constexpr auto LED_PIN{PICO_DEFAULT_LED_PIN};
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);

  clear(buffer, DISP_WIDTH * DISP_HEIGHT);

  const auto okay{dispInit(buffer, 1)};
  const auto okayoff{dispOff()};

  gpio_put(LED_PIN, okayoff ? 1 : 0);

  while (true) {
  }
}