#include "chippie/collision/collision.hpp"
#include "chippie/entities/entity_types.hpp"
#if !defined(STATE_HPP)
#define STATE_HPP

#include "chippie/chippie_common.hpp"
#include "chippie/entities/basic_entity.hpp"
#include "collision/collision.hpp"

#include "chippie/entities/purple_ball.hpp"

namespace chippie
{

struct State
{
  public:
    void load_level() noexcept
    {
        clear_collision_list();
        entity_list.clear();

        entity_list.push_back(create_purple_ball(Grid::Location{.x = 2, .y = 2}, direction::LEFT));
        entity_list.push_back(create_purple_ball(Grid::Location{.x = 2, .y = 2}, direction::LEFT));
    }
    void process() noexcept
    {
        move_entities();

        /* TODO event processing, which may change the size of entity_list */
        event::process_events();

        /* entity cleanup */
        auto first_dead_itr{
            std::partition(std::begin(entity_list), std::end(entity_list), [](const auto *ent) { return ent->alive; })};
        entity_list.resize(std::distance(std::begin(entity_list), first_dead_itr));

        /* paint the entire frame */
        paint_view_grid();
        paint_chip_count();
        paint_timer();
        paint_level();

        for (auto *ent : entity_list)
        {
            ent->paint();
        }

        if (event::exit())
        {
            return;
        }
    }

  private:
    void move_entities() noexcept
    {
        for (auto ent : entity_list)
        {
            const auto id{ent.identity};
            const auto nextloc{next_loc_ftable[id](ent)};
            const collision_status collision{check_for_collision(ent, nextloc)};
            switch (collision_ftable[id](ent, collision))
            {
            case collision_action::NO_ACTION_NEEDED:
                break;
            case collision_action::APPLY_NEXT_MOVE:
                ent.loc = nextloc;
            case collision_action::MARK_DEAD:
                ent.alive = false;
            }
        }
    }

    std::array<entity, 128> entity_list;

    std::array<compute_next_fun_t, 128> next_loc_ftable{purple_ball::compute_next_location};
    std::array<handle_collision_fun_t, 128> collision_ftable{purple_ball::handle_collision};
};

} // namespace chippie

#endif