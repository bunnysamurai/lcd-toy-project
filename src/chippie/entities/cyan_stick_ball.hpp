#if !defined(CYAN_STICK_BALL_HPP)
#define CYAN_STICK_BALL_HPP

#include "basic_entity.hpp"
#include "chippie/events/event.hpp"
#include "chippie/state/static_map.hpp"

namespace chippie::cyan_stick_ball
{

[[nodiscard]] entity create(state &game_state, Grid::Location, direction, uint8_t, uint64_t next_time = 0) noexcept;
[[nodiscard]] entity_state_machine get_state_functions() noexcept;

/**
    @brief Retrieve velocity for moves of a cyan_stick ball.

    @return Period between individual moves, in microseconds.
 */
[[nodiscard]] uint64_t get_velocity() noexcept;

} // namespace chippie::cyan_stick_ball

#endif