#if !defined(BACTERIA_HPP)
#define BACTERIA_HPP

#include "basic_entity.hpp"
#include "chippie/events/event.hpp"
#include "chippie/state/state.hpp"

namespace chippie::bacteria
{

[[nodiscard]] entity create(state &game_state, Grid::Location xy, direction dir, uint8_t uuid, uint64_t next_time = 0) noexcept;
[[nodiscard]] entity_state_machine get_state_functions() noexcept;

/**
    @brief Retrieve velocity for moves of a purple ball.

    @return Period between individual moves, in microseconds.
 */
[[nodiscard]] uint64_t get_velocity() noexcept;

} // namespace chippie::bacteria

#endif