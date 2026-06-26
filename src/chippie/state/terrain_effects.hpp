#if !defined(TERRAIN_EFFECTS_HPP)
#define TERRAIN_EFFECTS_HPP

#include "chippie/entities/basic_entity.hpp"
#include "chippie/state/state.hpp"
#include "chippie/state/terrain_types.hpp"

namespace chippie
{

void apply_terrain_entry_effect(entity &ent, terrain_type terrain) noexcept;
void apply_terrain_exit_effect(entity &ent, terrain_type terrain) noexcept;

/**
    @brief Check for "opacity" for general entities.  Chippie will do it's own thing.
 */
[[nodiscard]] bool check_terrain_is_opaque(entity &ent, terrain_type terrain) noexcept;

/* dir is the direction the moveable block is moving */
[[nodiscard]] bool check_terrain_is_opaque_for_moveable(direction dir, terrain_type terrain) noexcept;

} // namespace chippie
#endif