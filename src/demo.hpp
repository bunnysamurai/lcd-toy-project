#if !defined(DEMO_HPP)
#define DEMO_HPP

#if 0
#include "screen/waveshare_driver/dispWaveshareLcd.h"
#endif
#include <cstdint>

namespace demo
{

void run_text_animation() noexcept;

void run_color_rando_art() noexcept;

void run_linebounce_screensaver() noexcept;

#if 0
struct TouchConfig {
  uint32_t touch_poll_interval_ms{10};
  dispTouchCfg_t touchcfg;
};
void run_touch_demo(TouchConfig cfg = {.touch_poll_interval_ms = 1,
                                       .touchcfg = {.touch_zthresh = 0xFA0,
                                                    .first_toss = 10,
                                                    .last_toss = 1}}) noexcept;

#endif
} // namespace demo
#endif