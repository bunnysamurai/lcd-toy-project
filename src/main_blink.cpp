#include "pico/stdlib.h"

int main() {

  // #ifndef PICO_DEFAULT_LED_PIN
  // #warning "No default pin for the LED on this board type"
  // #else
  static constexpr auto LED_PIN{PICO_DEFAULT_LED_PIN};
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);
  while(true)
  {
    gpio_put(LED_PIN, 1);
    sleep_ms(1000);

    gpio_put(LED_PIN, 0);
    sleep_ms(1000);

  }
  // #endif
}