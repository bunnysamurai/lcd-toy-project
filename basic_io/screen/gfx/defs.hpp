#if !defined(SCREEN_GFX_DEFS_HPP)
#define SCREEN_GFX_DEFS_HPP

#include <cstdint>

namespace screen::gfx {
struct Point {
  uint32_t x;
  uint32_t y;

  [[nodiscard]] constexpr bool operator==(const Point&) const noexcept = default;
  [[nodiscard]] constexpr bool operator!=(const Point&) const noexcept = default;
};

struct Size {
  uint32_t width;
  uint32_t height;

  [[nodiscard]] constexpr bool operator==(const Size&) const noexcept = default;
  [[nodiscard]] constexpr bool operator!=(const Size&) const noexcept = default;
};

struct Rect {
  Point topleft;
  Size size;

  [[nodiscard]] constexpr bool operator==(const Rect&) const noexcept = default;
  [[nodiscard]] constexpr bool operator!=(const Rect&) const noexcept = default;
};

} // namespace screen::gfx
#endif