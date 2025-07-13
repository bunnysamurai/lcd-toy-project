#if !defined(SCREEN_DEF_H)
#define SCREEN_DEF_H

#include <cstddef>
#include <cstdint>

namespace screen {

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
constexpr uint32_t PHYSICAL_WIDTH_PIXELS{240U};
constexpr uint32_t PHYSICAL_HEIGHT_PIXELS{320U};

} // namespace screen

#endif