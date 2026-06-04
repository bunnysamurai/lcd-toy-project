#if !defined(SCREEN_GFX_DEFS_HPP)
#define SCREEN_GFX_DEFS_HPP

#include <cstdint>

namespace screen::gfx
{
template <typename T> struct Point_
{
    T x;
    T y;

    [[nodiscard]] constexpr bool operator==(const Point_ &) const noexcept = default;
    [[nodiscard]] constexpr bool operator!=(const Point_ &) const noexcept = default;
};

template <typename T> struct Size_
{
    T width;
    T height;

    [[nodiscard]] constexpr bool operator==(const Size_ &) const noexcept = default;
    [[nodiscard]] constexpr bool operator!=(const Size_ &) const noexcept = default;
};

template <typename T> struct Rect_
{
    Point_<T> topleft;
    Size_<T> size;

    [[nodiscard]] constexpr bool operator==(const Rect_ &) const noexcept = default;
    [[nodiscard]] constexpr bool operator!=(const Rect_ &) const noexcept = default;

    [[nodiscard]] constexpr bool contains(Point_<T> pt) noexcept
    {
        return pt.y >= topleft.y && pt.x >= topleft.x && pt.y < (topleft.y + size.height) &&
               pt.x < (topleft.x + size.width);
    }
};

using Point = Point_<uint32_t>;
using Size = Size_<uint32_t>;
using Rect = Rect_<uint32_t>;

} // namespace screen::gfx
#endif