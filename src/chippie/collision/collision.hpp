#if !defined(COLLISION_HPP)
#define COLLISION_HPP

#include "common/Grid.hpp"
#include "entities/basic_entity.hpp"
#include "entities/entity_types.hpp"
#include "state/static_map.hpp"

namespace chippie
{

enum struct collision_action
{
    NO_ACTION_NEEDED,
    MOVE_FORWARD,
    MOVE_BACKWARD,
    MOVE_RIGHT,
    MOVE_LEFT,
    APPLY_NEXT_MOVE,
    MARK_DEAD
};

struct collision_result
{
    entity* other;
    terrain_type& tile;
};

} // namespace chippie
#endif