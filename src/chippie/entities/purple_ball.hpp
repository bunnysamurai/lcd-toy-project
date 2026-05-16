#if !defined(PURPLE_BALL_HPP)
#define PURPLE_BALL_HPP

#include "basic_entity.hpp"
#include "chippie/chippie_common.hpp"
#include "chippie/collision.hpp"
#include "chippie/events/event.hpp"
#include "chippie/state/static_map.hpp"

namespace chippie
{
namespace purple_ball
{
[[nodiscard]] entity create(Grid::Location, direction) noexcept;
[[nodiscard]] Grid::Location compute_next_location(entity ent) noexcept;
[[nodiscard]] collision_action handle_collision(const entity &ent, entity *collided_entity,
                                                terrain_type &collided_terrain) noexcept;

} // namespace purple_ball

} // namespace chippie

#endif