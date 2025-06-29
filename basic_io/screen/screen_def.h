#if !defined(SCREEN_DEF_H)
#define SCREEN_DEF_H

#include <cstddef>
#include <cstdint>

namespace screen {
struct Position {
  uint32_t row;
  uint32_t column;
};
struct Dimensions {
  uint32_t width;
  uint32_t height;
};

#ifdef WAVESHARE_240P
enum struct Format { GREY1, GREY2, GREY4, RGB565_LUT8, RGB565 };

[[nodiscard]] constexpr size_t bitsizeof(Format fmt) {
  switch (fmt) {
  case Format::GREY1:
    return 1U;
  case Format::GREY2:
    return 2U;
  case Format::GREY4:
    return 4U;
  case Format::RGB565_LUT8:
    return 8U;
  case Format::RGB565:
    return 16U;
  }
  return 0U;
}
constexpr Dimensions PHYSICAL_SIZE{.width = 240, .height = 320};
#endif
} // namespace screen

#endif