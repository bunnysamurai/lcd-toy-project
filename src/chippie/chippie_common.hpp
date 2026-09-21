#if !defined(CHIPPIE_COMMON_HPP)
#define CHIPPIE_COMMON_HPP

#include "common/pico_sdk_clock_details.hpp"
#include "common/time_utils.hpp"
#include "utils/Grid.hpp"

#include <cstdint>
#include <utility>

namespace chippie
{

using steady_clock_source = clock_details::pico_sdk_steady_clock;
using game_clock_t = embp::pauseable_clock<steady_clock_source>;

enum struct direction
{
    UP,
    RIGHT,
    DOWN,
    LEFT,
};

constexpr int NO_TIME_LIMIT{-1};

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
    return facing;
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
    return facing;
}

[[nodiscard]] constexpr direction rotate_anticlockwise(direction facing) noexcept
{
    switch (facing)
    {
    case direction::UP:
        return direction::LEFT;
    case direction::DOWN:
        return direction::RIGHT;
    case direction::RIGHT:
        return direction::UP;
    case direction::LEFT:
        return direction::DOWN;
    }
    return facing;
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

[[nodiscard]] constexpr direction to_abs_direction(direction facing, relative_direction input) noexcept
{
    switch (input)
    {
    case relative_direction::FORWARD:
        return facing;
    case relative_direction::BACKWARD:
        return reverse(facing);
    case relative_direction::RIGHT:
        return rotate_clockwise(facing);
    case relative_direction::LEFT:
        return rotate_anticlockwise(facing);
    }
}

static_assert(direction::UP == to_abs_direction(direction::UP, relative_direction::FORWARD));
static_assert(direction::DOWN == to_abs_direction(direction::UP, relative_direction::BACKWARD));
static_assert(direction::RIGHT == to_abs_direction(direction::UP, relative_direction::RIGHT));
static_assert(direction::LEFT == to_abs_direction(direction::UP, relative_direction::LEFT));

static_assert(direction::DOWN == to_abs_direction(direction::DOWN, relative_direction::FORWARD));
static_assert(direction::UP == to_abs_direction(direction::DOWN, relative_direction::BACKWARD));
static_assert(direction::LEFT == to_abs_direction(direction::DOWN, relative_direction::RIGHT));
static_assert(direction::RIGHT == to_abs_direction(direction::DOWN, relative_direction::LEFT));

static_assert(direction::RIGHT == to_abs_direction(direction::RIGHT, relative_direction::FORWARD));
static_assert(direction::LEFT == to_abs_direction(direction::RIGHT, relative_direction::BACKWARD));
static_assert(direction::DOWN == to_abs_direction(direction::RIGHT, relative_direction::RIGHT));
static_assert(direction::UP == to_abs_direction(direction::RIGHT, relative_direction::LEFT));

static_assert(direction::LEFT == to_abs_direction(direction::LEFT, relative_direction::FORWARD));
static_assert(direction::RIGHT == to_abs_direction(direction::LEFT, relative_direction::BACKWARD));
static_assert(direction::UP == to_abs_direction(direction::LEFT, relative_direction::RIGHT));
static_assert(direction::DOWN == to_abs_direction(direction::LEFT, relative_direction::LEFT));

} // namespace chippie

#endif