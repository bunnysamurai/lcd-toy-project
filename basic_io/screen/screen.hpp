#if !defined(SCREEN_H)
#define SCREEN_H

#include <cstdint>

#include "pico/time.h"

#if defined(WAVESHARE_240P)
#include "waveshare_driver/screen_def.h"
#endif

namespace screen {
struct Position {
  uint32_t row;
  uint32_t column;
};
struct Dimensions {
  uint32_t width;
  uint32_t height;
};

[[nodiscard]] bool init(const uint8_t *video_buf, Position virtual_topleft,
                        Dimensions virtual_size, Format format) noexcept;

[[nodiscard]] const uint8_t *get_video_buffer() noexcept;
void set_video_buffer(const uint8_t *buffer) noexcept;

[[nodiscard]] Format get_format() noexcept;
void set_format(Format) noexcept;

[[nodiscard]] Dimensions get_virtual_screen_size() noexcept;
void set_virtual_screen_size(Position new_topleft,
                             Dimensions new_size) noexcept;

[[nodiscard]] constexpr Dimensions get_physical_screen_size() noexcept {
  return {.width = PHYSICAL_WIDTH_PIXELS, .height = PHYSICAL_HEIGHT_PIXELS};
}

struct TouchReport {
  int x;
  int y;
  bool pen_up; // indicate whether someone is touching the display or not
  absolute_time_t timestamp;
};
/** @brief Get lateset touch report.
 *
 *  Check the timestamp, as these may be stale.
 *
 *  Once called, will emtpy the underlying ring buffer (it's only 1 element in
 * size).
 *
 * @param[out] out The latest report.  Don't use if return is false.
 *
 * @return True if a new report is availble, false otherwise.
 */
[[nodiscard]] bool get_touch_report(TouchReport &out);

} // namespace screen
#endif