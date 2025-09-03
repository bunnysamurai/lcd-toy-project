#include "gamepad.hpp"

#include <array>

#include "hardware/gpio.h"
#include "pico/time.h"

/* ======================================================================= *
                     ____        ____   _    ____
                    | ___|      |  _ \ / \  |  _ \
                    |___ \ _____| |_) / _ \ | | | |
                     ___) |_____|  __/ ___ \| |_| |
                    |____/      |_| /_/   \_\____/

 * ======================================================================= */
namespace gamepad::five {

/* some defaults */
static constexpr int64_t DEFAULT_POLL_INTERVAL_US{10000}; /* ever 10 ms */

/* some bsp level stuff, to be abstracted away? This is for the breadboard
 * setup. */
static constexpr uint32_t BREADBOARD_PAD_UP{4};
static constexpr uint32_t BREADBOARD_PAD_DOWN{6};
static constexpr uint32_t BREADBOARD_PAD_RIGHT{7};
static constexpr uint32_t BREADBOARD_PAD_LEFT{3};
static constexpr uint32_t BREADBOARD_PAD_ETC{2};
static constexpr uint32_t GPIOS_USED{
    (1 << BREADBOARD_PAD_UP) | (1 << BREADBOARD_PAD_DOWN) |
    (1 << BREADBOARD_PAD_RIGHT) | (1 << BREADBOARD_PAD_LEFT) |
    (1 << BREADBOARD_PAD_ETC)};

struct PadControl {
  int64_t poll_interval;
  alarm_id_t id;
  uint32_t gpio_pins_state;
};

/* our static state for the module*/
bool g_initialized{false};
static PadControl g_control{
    .poll_interval = DEFAULT_POLL_INTERVAL_US,
};

static int64_t polling_callback(alarm_id_t id, void *user_data) {
  PadControl *p_state = reinterpret_cast<PadControl *>(user_data);
  if (id != p_state->id) {
    return 0;
  }

  /* read our pins */
  p_state->gpio_pins_state = gpio_get_all();

  return -(p_state->poll_interval); /* resets the alarm */
}

static void configure_timer() noexcept {
  alarm_pool_init_default();
  g_control.id = add_alarm_in_us(g_control.poll_interval, polling_callback,
                                 &g_control, false);
}

static void configure_gpios() noexcept {
  /* always set for input, disable pull-up/pull-down */

  gpio_set_function_masked(GPIOS_USED, GPIO_FUNC_SIO);
  gpio_set_dir_in_masked(GPIOS_USED);

  uint32_t gpio_pin{GPIOS_USED};
  for (int shift = 0; shift < 32; ++shift) {
    if (gpio_pin & 0b1) {
      gpio_set_pulls(shift, false, false);
    }
    gpio_pin >>= 1;
  }
}

/* we're going to use a timer to periodically poll */
void init() noexcept {
  if (!g_initialized) {
    configure_gpios();
    configure_timer();
    g_initialized = true;
  }
}

void deinit() noexcept {
  if (g_initialized) {
    /* we'll leave the gpios as-is, but we'll disable our running timer */
    cancel_alarm(g_control.id);
    g_initialized = false;
  }
}

[[nodiscard]] State get() noexcept {

  const uint32_t current_pins{g_control.gpio_pins_state};

  auto &&is_clear{
      [](const uint32_t pins_state, const uint32_t gpio_num) -> uint8_t {
        return static_cast<uint8_t>((pins_state >> gpio_num) & 0b1) == 0;
      }};

  return {.up = is_clear(current_pins, BREADBOARD_PAD_UP),
          .down = is_clear(current_pins, BREADBOARD_PAD_DOWN),
          .right = is_clear(current_pins, BREADBOARD_PAD_RIGHT),
          .left = is_clear(current_pins, BREADBOARD_PAD_LEFT),
          .etc = is_clear(current_pins, BREADBOARD_PAD_ETC)};
}

} // namespace gamepad::five

/* ======================================================================= *
                     ___        ____   _    ____
                    ( _ )      |  _ \ / \  |  _ \
                    / _ \ _____| |_) / _ \ | | | |
                   | (_) |_____|  __/ ___ \| |_| |
                    \___/      |_| /_/   \_\____/

 * ======================================================================= */
namespace gamepad::eight {}