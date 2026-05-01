#if !defined(CHIPPIE_COMMON_HPP)
#define CHIPPIE_COMMON_HPP

#include "common/Grid.hpp"

namespace chippie
{

enum struct direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

[[nodiscard]] constexpr direction reverse(direction facing) noexcept
{
    switch (facing)
    {
    case direction::UP:
        return direction::DOWN;
    case direction::DOWN:
        return direction::UP;
    case direction::RIGHT:
        return direction::LEFT;
    case direction::LEFT:
        return direction::RIGHT;
    }
}

enum struct relative_direction
{
    FORWARD,
    BACKWARD,
    RIGHT,
    LEFT
};


} // namespace chippie

#endif