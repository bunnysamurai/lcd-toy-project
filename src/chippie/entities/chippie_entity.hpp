#if !defined(CHIPPIE_ENTITY_HPP)
#define CHIPPIE_ENTITY_HPP

#include <cstdint>

#include "basic_entity.hpp"
#include "chippie/chippie_common.hpp"
#include "chippie/state/state.hpp"
#include "common/Grid.hpp"

namespace chippie::chippie
{

[[nodiscard]] entity create(state &game_state, Grid::Location, direction, uint8_t) noexcept;

[[nodiscard]] entity_state_machine get_state_functions() noexcept;

/**
    @brief Retrieve velocity for moves of Chippie

    @return Period between individual moves, in microseconds.
 */
[[nodiscard]] uint64_t get_velocity() noexcept;

} // namespace chippie::chippie

#endif