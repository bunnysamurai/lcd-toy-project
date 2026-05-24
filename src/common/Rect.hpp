#ifndef RECT_HPP
#define RECT_HPP

template <typename T> struct Rect_
{
    T x;
    T y;
    T width;
    T height;

    [[nodiscard]] constexpr bool contains(T px, T py) noexcept
    {
        return py >= y && px >= x && py < (y + height) && px < (x + width);
    }
};

using Rect = Rect_<int>;

#endif