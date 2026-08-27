#if !defined(WATER_GLIDER_HPP)
#define WATER_GLIDER_HPP

#include "basic_entity.hpp"
#include "chippie/events/event.hpp"
#include "chippie/state/static_map.hpp"

namespace chippie::water_glider
{

[[nodiscard]] entity create(state &game_state, Grid::Location, direction, uint8_t, uint64_t next_time = 0) noexcept;
[[nodiscard]] entity_state_machine get_state_functions() noexcept;

/**
    @brief Retrieve velocity for moves of a fire dancer.

    @return Period between individual moves, in microseconds.
 */
[[nodiscard]] uint64_t get_velocity() noexcept;

} // namespace chippie::water_glider

#endif