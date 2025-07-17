#if !defined (CONSTEXPR_TILE_UTILS_HPP)
#define CONSTEXPR_TILE_UTILS_HPP

#include <array>
#include <cstddef>
#include <cstdint>

#include "TileDef.h"

namespace constexpr_screen
{

enum struct Color {
  WHITE,
  BLACK,
  GREY,
  RED,
  GREEN,
  BLUE,
  MAGENTA,
  CYAN,
  YELLOW
};

[[nodiscard]] constexpr uint16_t to_rgb565(Color clr) noexcept {
  switch (clr) {
  case Color::WHITE:
    return 0xFFFFU;
  case Color::BLACK:
    return 0x0000U;
  case Color::GREY:
    return 0xFFFFU >> 1;
  case Color::RED:
    return 0b11111'000000'00000;
  case Color::GREEN:
    return 0b00000'111111'00000;
  case Color::BLUE:
    return 0b00000'000000'11111;
  case Color::MAGENTA:
    return 0b11111'000000'11111;
  case Color::CYAN:
    return 0b00000'111111'11111;
  case Color::YELLOW:
    return 0b11111'111111'00000;
  }
  return 0;
}

template <size_t N>
[[nodiscard]] constexpr std::array<uint8_t, N> fill_with_rgb565(Color clr) noexcept {
  static_assert((N & 0x1) == 0);
  std::array<uint8_t, N> result;
  for (size_t idx{0}; idx < N; idx += 2) {
    const uint16_t bytes{to_rgb565(clr)};
    /* little-endian, I think?? */
    result[idx] = bytes & 0x00FFU;
    result[idx + 1] = (bytes & 0xFF00U) >> 8;
  }
  return result;
}

}

#endif