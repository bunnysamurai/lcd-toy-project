#if !defined(PURPLE_BALL_HPP)
#define PURPLE_BALL_HPP

#include "basic_entity.hpp"
#include "chippie/chippie_common.hpp"
#include "chippie/collision.hpp"
#include "chippie/events/event.hpp"

namespace chippie
{
namespace purple_ball
{
[[nodiscard]] entity create(Grid::Location, direction) noexcept;
[[nodiscard]] Grid::Location compute_next_location(entity ent) noexcept;
collision_action handle_collision(const entity &ent, collision_status collision) noexcept;

} // namespace purple_ball

} // namespace chippie

#endif