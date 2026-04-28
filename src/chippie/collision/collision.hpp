#if !defined(COLLISION_HPP)
#define COLLISION_HPP

#include "common/Grid.hpp"
#include "entities/basic_entity.hpp"
#include "entities/entity_types.hpp"
#include "state/static_map.hpp"

namespace chippie
{

struct collision_status
{
    entity_type type;
    Grid::Location position;
};

[[nodiscard]] collision_status check_for_collision(const entity *this_entity, Grid::Location next_location) noexcept;

/* maintain a linked list of living entities, this is initialzed on every level load */
void clear_collision_list() noexcept;
void add_entity_to_collision_list(const entity *) noexcept;
void remove_entity_from_collision_list(const entity *) noexcept;

/* we also need a handle to the static-entity map, which is also passed in by reference */
void register_static_map(const static_map *) noexcept;

} // namespace chippie
#endif