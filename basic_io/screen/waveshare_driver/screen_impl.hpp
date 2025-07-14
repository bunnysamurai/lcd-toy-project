#if !defined(SCREEN_IMPL_H)
#define SCREEN_IMPL_H

#include <cstdint>

#include "pico/time.h"

#include "../screen_def.h"

namespace screen_impl {

using ::screen::Dimensions;
using ::screen::Format;
using ::screen::Position;
using ::screen::TouchReport;

[[nodiscard]] bool init(const uint8_t *video_buf, Position virtual_topleft,
                        Dimensions virtual_size, Format format) noexcept;

[[nodiscard]] const uint8_t *get_video_buffer() noexcept;
void set_video_buffer(const uint8_t *buffer) noexcept;

[[nodiscard]] Format get_format() noexcept;
void set_format(Format) noexcept;

[[nodiscard]] Dimensions get_virtual_screen_size() noexcept;
void set_virtual_screen_size(Position new_topleft,
                             Dimensions new_size) noexcept;

[[nodiscard]] bool get_touch_report(TouchReport &out);

} // namespace screen_impl
#endif