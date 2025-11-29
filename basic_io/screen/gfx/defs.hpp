#if !defined(SCREEN_GFX_DEFS_HPP)
#define SCREEN_GFX_DEFS_HPP

#include <cstdint>

namespace screen::gfx {
struct Point {
  uint32_t x;
  uint32_t y;
};
struct Size {
  uint32_t width;
  uint32_t height;
};
struct Rect {
  Point topleft;
  Size size;
};
} // namespace screen::gfx
#endif