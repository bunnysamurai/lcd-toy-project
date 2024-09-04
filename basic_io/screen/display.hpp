#if !defined(SCREEN_DISPLAY_H)
#define SCREEN_DISPLAY_H

#include <cstdint>

namespace screen
{
  struct Position
  {
    uint32_t row;
    uint32_t column;
  };
  struct Dimensions
  {
    uint32_t width;
    uint32_t height;
  };

  struct Format
  {
    uint8_t bpp; // bits per pixel
  };

  inline constexpr Dimensions PHYSICAL_SIZE{.width = 240, .height = 320};
  inline constexpr uint8_t MAX_SUPPORTED_BPP{16};

  [[nodiscard]] bool
  init(const uint8_t *video_buf, Position virtual_topleft, Dimensions virtual_size, Format format) noexcept;
  [[nodiscard]] const uint8_t *get_video_buffer() noexcept;
  void set_video_buffer(const uint8_t *buffer);
  [[nodiscard]] Format get_format() noexcept;
  [[nodiscard]] Dimensions get_virtual_screen_size() noexcept;
  void set_virtual_screen_size(Position new_topleft, Dimensions new_size) noexcept;

  [[nodiscard]] constexpr Dimensions get_physical_screen_size() noexcept
  {
    return PHYSICAL_SIZE;
  }
}
#endif