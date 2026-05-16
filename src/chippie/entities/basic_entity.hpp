#include "chippie/collision.hpp"
#if !defined(BASIC_ENTITY_HPP)
#define BASIC_ENTITY_HPP

#include "../chippie_common.hpp"
#include "../render/paint_utils.hpp"
#include "../render/view_grid.hpp"
#include "common/Grid.hpp"
#include "entity_types.hpp"

namespace chippie
{

/** entities are anything in the game that can "move"
    This includes purple balls, frog monsters, and bacteria, among others.
 */
struct entity
{
    Grid::Location loc;
    entity_type identity;
    uint16_t velocity_ticks;
    direction facing;
    bool alive{true};
    bool trapped{false};
};

using compute_next_fun_t = Grid::Location (*)(const entity &) noexcept;
using handle_collision_fun_t = collision_action (*)(entity &this_entity, entity *collided_entity,
                                                    terrain_type &collided_terrain) noexcept;

/* entity manipulating functions... leaving these free for now as that makes things less complicated.  I might be a C
 * programmer now :() */

void paint_entity(const entity &ent) const noexcept
{
    if (within_view_grid(ent.loc))
    {
        draw_tile_index(ent.loc, ent.identity);
    }
}
constexpr void move_relative_direction(entity &ent, relative_direction dir) noexcept
{
}

} // namespace chippie

#endif