#if !defined(CHIPPIE_COMMON_HPP)
#define CHIPPIE_COMMON_HPP

#include "common/Grid.hpp"

#include <cstdint>
#include <utility>

namespace chippie
{

enum struct direction
{
    UP,
    RIGHT,
    DOWN,
    LEFT,
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

[[nodiscard]] constexpr direction rotate_clockwise(direction facing) noexcept
{
    switch (facing)
    {
    case direction::UP:
        return direction::RIGHT;
    case direction::DOWN:
        return direction::LEFT;
    case direction::RIGHT:
        return direction::DOWN;
    case direction::LEFT:
        return direction::UP;
    }
}

[[nodiscard]] constexpr Grid::Location move(Grid::Location loc, direction facing) noexcept
{
    Grid::Location result{loc};
    switch (facing)
    {
    case direction::UP:
        --result.y;
        break;
    case direction::DOWN:
        ++result.y;
        break;
    case direction::RIGHT:
        ++result.x;
        break;
    case direction::LEFT:
        --result.x;
        break;
    }
    return result;
}

enum struct relative_direction
{
    FORWARD,
    RIGHT,
    BACKWARD,
    LEFT
};

[[nodiscard]] constexpr std::pair<Grid::Location, direction> move(Grid::Location loc, direction facing,
                                                                  relative_direction dir) noexcept
{
    const direction new_facing{(static_cast<uint8_t>(facing) + static_cast<uint8_t>(dir)) & 0b11};

    return std::make_pair(move(loc, new_facing), new_facing);
}

} // namespace chippie

#endif