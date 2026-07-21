#if !defined(BACTERIA_HPP)
#define BACTERIA_HPP

#include "basic_entity.hpp"
#include "chippie/events/event.hpp"
#include "chippie/state/static_map.hpp"

namespace chippie::bacteria
{

[[nodiscard]] entity create(state &game_state, Grid::Location, direction, uint8_t) noexcept;
[[nodiscard]] entity_state_machine get_state_functions() noexcept;

/**
    @brief Retrieve velocity for moves of a purple ball.

    @return Period between individual moves, in microseconds.
 */
[[nodiscard]] uint64_t get_velocity() noexcept;

} // namespace chippie::bacteria

#endif