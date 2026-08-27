#if !defined(PURPLE_BALL_HPP)
#define PURPLE_BALL_HPP

#include "basic_entity.hpp"
#include "chippie/events/event.hpp"
#include "chippie/state/static_map.hpp"

namespace chippie::purple_ball
{

[[nodiscard]] entity create(state &game_state, Grid::Location, direction, uint8_t, uint64_t next_time = 0) noexcept;
[[nodiscard]] entity_state_machine get_state_functions() noexcept;

/**
    @brief Retrieve velocity for moves of a purple ball.

    @return Period between individual moves, in microseconds.
 */
[[nodiscard]] uint64_t get_velocity() noexcept;

} // namespace chippie::purple_ball

#endif