#if !defined(MOVEABLE_BLOCK_ENTITY_HPP)
#define MOVEABLE_BLOCK_ENTITY_HPP

#include <cstdint>

#include "basic_entity.hpp"
#include "chippie/chippie_common.hpp"
#include "chippie/state/state.hpp"
#include "utils/Grid.hpp"

namespace chippie::moveable_block_entity
{

[[nodiscard]] entity create(state &game_state, Grid::Location, direction, uint8_t) noexcept;

[[nodiscard]] entity_state_machine get_state_functions() noexcept;

/**
    @brief Retrieve velocity for moves of Moveable Block, which is of course zero

    @return Period between individual moves, in microseconds.
 */
[[nodiscard]] uint64_t get_velocity() noexcept;

} // namespace chippie::chippie

#endif