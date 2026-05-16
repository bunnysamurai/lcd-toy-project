#if !defined(TERRAIN_EFFECTS_HPP)
#define TERRAIN_EFFECTS_HPP

#include "chippie/entities/basic_entity.hpp"
#include "static_map.hpp"

namespace chippie
{

void apply_terrain_effect(entity &ent, terrain_type terrain) noexcept;

}
#endif