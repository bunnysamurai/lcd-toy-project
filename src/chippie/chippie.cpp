#if !defined(EVENT_HPP)
#define EVENT_HPP

#include <algorithm>
#include <array>
#include <cstdint>

#include "chippie_common.hpp"
#include "entities/basic_entity.hpp"
#include "entities/purple_ball.hpp"
#include "events/event.hpp"

#include "embp/variable_array.hpp"

namespace chippie
{

void run()
{

    /* this is definitely apart of the generic game state */
    embp::variable_array<entity *, 128> entity_list;

    /* load the "level" */
    std::array<purple_ball, 2> pballs{
        purple_ball{Grid::Location{.x = 2, .y = 2}, direction::LEFT},
        purple_ball{Grid::Location{.x = 2, .y = 3}, direction::LEFT},
    };
    std::for_each(std::begin(pballs), std::end(pballs), [&](auto &c) { entity_list.push_back(std::addressof(c)); });

    /* example iteration of the game loop */

    /* process entities (i.e move them) */
    for (auto *ent : entity_list)
    {
        ent->process();
    }

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

} // namespace chippie
#endif