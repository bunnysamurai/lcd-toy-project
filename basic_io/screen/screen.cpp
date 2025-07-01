#include "screen.hpp"

#include "screen_def.h"

// TODO consider not using the Pico SDK?
#include "pico/printf.h"
#include "pico/stdlib.h"

#include "waveshare_driver/dispWaveshareLcd.h"
#include "waveshare_driver/pinout.h"

#include "embp/circular_array.hpp"

namespace screen {
namespace {

TouchReport s_latest_touch_report;

void setup_for_input(uint id) noexcept {
  gpio_init(id);
  gpio_set_dir(id, false);
}
void setup_for_output(uint id) noexcept {
  gpio_init(id);
  gpio_set_dir(id, true);
}

[[nodiscard]] constexpr bool
range_check_dimensions(Dimensions testdim) noexcept {
  return testdim.width <= PHYSICAL_SIZE.width &&
         testdim.height <= PHYSICAL_SIZE.height;
}

} // namespace

void set_format(Format fmt) noexcept { dispSetDepth(bitsizeof(fmt)); }

Format get_format() noexcept {
  const auto bpp = dispGetDepth();
  switch (bpp) {
  case 1U:
    return Format::GREY1;
  case 2U:
    return Format::GREY2;
  case 4U:
    return Format::GREY4;
  case 8U:
    return Format::RGB565_LUT8;
  case 16U:
    return Format::RGB565;
  }
  return Format::RGB565;
}
Dimensions get_virtual_screen_size() noexcept {
  const auto dims{dispGetVirtualDimensions()};
  return {.width = dims.width, .height = dims.height};
}
void set_virtual_screen_size([[maybe_unused]] Position new_topleft,
                             Dimensions new_size) noexcept {
  dispSetVirtualDimensions(
      {.width = new_size.width, .height = new_size.height});
}

const uint8_t *get_video_buffer() noexcept { return dispGetVideoBuffer(); }

bool init(const uint8_t *video_buf, [[maybe_unused]] Position virtual_topleft,
          Dimensions virtual_size, Format format) noexcept {
  if (!range_check_dimensions(virtual_size)) {
    return false;
  }

  bool status{true};

  setup_for_output(PIN_TOUCH_CS);
  setup_for_output(PIN_LCD_RESET);
  setup_for_output(PIN_LCD_DnC);
  setup_for_output(PIN_LCD_CS);
  setup_for_output(PIN_SPI_CLK);
  setup_for_output(PIN_SPI_MOSI);
  setup_for_input(PIN_SPI_MISO);
  setup_for_output(PIN_LCD_BL);

  status &=
      dispInit(video_buf, bitsizeof(format),
               {.width = virtual_size.width, .height = virtual_size.height},
               {.width = PHYSICAL_SIZE.width, .height = PHYSICAL_SIZE.height});

  gpio_put(PIN_LCD_BL, true);

  return status;
}

void set_video_buffer(const uint8_t *buffer) noexcept {
  dispSetVideoBuffer(buffer);
}

static embp::circular_array<TouchReport, 1> s_touch_ring(1);
[[nodiscard]] bool get_touch_report(TouchReport &out) {

  if (s_touch_ring.empty()) {
    return false;
  }
  out = s_touch_ring.front();
  s_touch_ring.pop_front();
  return true;
}

/** @brief Hook into DmitryGR's Waveshare LCD/touchscreen driver
 *
 * This function get's called periodically within an interrupt.
 * So, you know, don't tarry.
 *
 * Now that I understand how the touch driver works, here's what
 * you need to know.
 *
 * This callback ONLY gets called when someone is pushing the screen.
 * So it seems that when this happens, we push the latest active sample
 * into a ring buffer with a timestamp.
 *
 * @param x Column location of next sample, negative on pen up
 * @param y Row location of next sample, negative on pen up
 */
extern "C" {
void dispExtTouchReport(int16_t x, int16_t y) {
  const auto report{TouchReport{.x = x,
                                .y = y,
                                .pen_up = x < 0 || y < 0,
                                .timestamp = get_absolute_time()}};
  s_touch_ring.push_back(report);
}
}

} // namespace screen
