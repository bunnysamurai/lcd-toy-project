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

struct entity
{
    Grid::Location loc;
    entity_type identity;
    uint16_t velocity_ticks;
    direction facing;
    bool alive{true};
};

using compute_next_fun_t = Grid::Location (*)(const entity &) noexcept;
using handle_collision_fun_t = collision_action (*)(const entity &, collision_status) noexcept;

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