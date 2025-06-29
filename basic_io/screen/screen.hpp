#if !defined(SCREEN_H)
#define SCREEN_H

#include <cstdint>

#include "screen_def.h"

namespace screen {

[[nodiscard]] bool init(const uint8_t *video_buf, Position virtual_topleft,
                        Dimensions virtual_size, Format format) noexcept;

[[nodiscard]] const uint8_t *get_video_buffer() noexcept;
void set_video_buffer(const uint8_t *buffer);

[[nodiscard]] Format get_format() noexcept;
void set_format(Format) noexcept;

[[nodiscard]] Dimensions get_virtual_screen_size() noexcept;
void set_virtual_screen_size(Position new_topleft,
                             Dimensions new_size) noexcept;

[[nodiscard]] constexpr Dimensions get_physical_screen_size() noexcept {
  return PHYSICAL_SIZE;
}
} // namespace screen
#endif