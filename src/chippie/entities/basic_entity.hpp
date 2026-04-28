#include "chippie/collision.hpp"
#if !defined(BASIC_ENTITY_HPP)
#define BASIC_ENTITY_HPP

#include "../paint_utils.hpp"
#include "../view_grid.hpp"
#include "common/Grid.hpp"
#include "entity_types.hpp"

namespace chippie
{

struct entity
{
  public:
    Grid::Location loc;
    entity_type identity;
    bool alive;

    entity(Grid::Location location, entity_type id) noexcept : loc{location}, identity{id}, alive{true}
    {
    }

    void process() noexcept
    {
        /* compute where the next location will be
            How this computation is done is entity specific
        */
        const auto nextloc{compute_next_location()};
        const collision_status collision{check_for_collision(this, nextloc)};
        /* if there is a collision, handle it, which is specific to each entity type */
        handle_collision(collision, nextloc);
    }

    void paint() const noexcept
    {
        if (within_view_grid(loc))
        {
            draw();
        }
    }

  protected:
    [[nodiscard]] virtual Grid::Location compute_next_location() const noexcept = 0;
    virtual void handle_collision(collision_status, Grid::Location) noexcept = 0;
    virtual void draw() const noexcept
    {
        draw_tile_index(loc, identity);
    };
};

} // namespace chippie

#endif