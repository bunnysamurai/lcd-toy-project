#if !defined(SCREEN_DEF_H)
#define SCREEN_DEF_H

#include <cstddef>
#include <cstdint>

/* #include "pico/time.h" */

namespace screen {

struct Position {
  uint32_t row;
  uint32_t column;
};
struct Dimensions {
  uint32_t width;
  uint32_t height;
};

struct Clut {
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

struct TouchReport {
  int x;
  int y;
  bool pen_up; // indicate whether someone is touching the display or not
  /* absolute_time_t timestamp; */
};

enum struct Format { GREY1, GREY2, GREY4, RGB565_LUT4, RGB565_LUT8, RGB565 };

[[nodiscard]] constexpr size_t bitsizeof(Format fmt) {
  switch (fmt) {
  case Format::GREY1:
    return 1U;
  case Format::GREY2:
    return 2U;
  case Format::GREY4:
  case Format::RGB565_LUT4:
    return 4U;
  case Format::RGB565_LUT8:
    return 8U;
  case Format::RGB565:
    return 16U;
  }
  return 0U;
}

/** @brief log2(bitsizeof(fmt)) */
[[nodiscard]] constexpr size_t subbyte_index(uint32_t pixpos, Format fmt) {
  switch (fmt) {
  case Format::GREY1:
    return pixpos & 0b111;
  case Format::GREY2:
    return pixpos & 0b11;
  case Format::GREY4:
  case Format::RGB565_LUT4:
    return pixpos & 0b1;
  case Format::RGB565_LUT8:
    return 0;
  case Format::RGB565:
    return 0;
  }
  return 0;
}

/** @brief Byte-aligned offset that contains this pixel position. */
[[nodiscard]] constexpr size_t align_byte(uint32_t position,
                                          Format fmt) noexcept {
  switch (fmt) {
  case Format::GREY1:
    return position >> 3;
  case Format::GREY2:
    return position >> 2;
  case Format::GREY4:
  case Format::RGB565_LUT4:
    return position >> 1;
  case Format::RGB565_LUT8:
    return position;
  case Format::RGB565:
    return position << 1;
  }
  return 0;
}

[[nodiscard]] constexpr uint8_t expand(uint32_t value, Format fmt) noexcept {
  switch (fmt) {
  case Format::GREY1:
    return value ? 0xFF : 0x00;
  case Format::GREY2:
    return value | (value << 2) | (value << 4) | (value << 6);
  case Format::GREY4:
  case Format::RGB565_LUT4:
    return value | (value << 4);
  case Format::RGB565_LUT8:
    return value;
  case Format::RGB565:
    return static_cast<uint8_t>(value);
  }
  return 0;
}

} // namespace screen

#endif