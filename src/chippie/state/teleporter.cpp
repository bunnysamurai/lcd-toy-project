#include "teleporter.hpp"

#include "chippie/entities/basic_entity.hpp"
#include "chippie/state/state.hpp"
#include "chippie/state/terrain_effects.hpp"

#include <algorithm>
#include <iterator>

namespace chippie
{

/*========================================================================================*/
/*                          ____  _        _   _                                          */
/*                         / ___|| |_ __ _| |_(_) ___ ___                                 */
/*                         \___ \| __/ _` | __| |/ __/ __|                                */
/*                          ___) | || (_| | |_| | (__\__ \                                */
/*                         |____/ \__\__,_|\__|_|\___|___/                                */
/*                                                                                        */
/*========================================================================================*/
namespace
{
[[nodiscard]] constexpr uint32_t to_index(direction entry) noexcept
{
    switch (entry)
    {
    case direction::UP:
        return 0U;
    case direction::RIGHT:
        return 1U;
    case direction::DOWN:
        return 2U;
    case direction::LEFT:
        return 3U;
    }
    return 0U;
}
} // namespace

/*========================================================================================*/
/*========================================================================================*/
void teleporter::set_exit(direction entry_facing, Grid::Location exit_location, direction exit_facing) noexcept
{
    exit_locations[to_index(entry_facing)] = exit_location;
    exit_facings[to_index(entry_facing)] = exit_facing;
}

[[nodiscard]] std::pair<direction, Grid::Location> teleporter::compute_exit(direction entry_facing) const noexcept
{
    return std::make_pair(exit_facings[to_index(entry_facing)], exit_locations[to_index(entry_facing)]);
}

/*========================================================================================*/
/*========================================================================================*/
std::pair<direction, Grid::Location> teleporter_chain::compute_exit(const entity &this_ent,
                                                                    Grid::Location entry_location,
                                                                    direction entry_facing) const noexcept
{
    auto &&advance_with_wrap{[&](auto &itr) {
        ++itr;
        if (itr == std::cend(locations))
        {
            itr = std::cbegin(locations);
        }
    }};

    /* we need this in order to inspect the static map */
    const state &game_state{*this_ent.game_state};

    /* first, find out where we are... by definition, we should find a match... logic error otherwise */
    const auto stop_iter{std::find_if(std::cbegin(locations), std::cend(locations),
                                      [=](const auto candidate_loc) { return candidate_loc == entry_location; })};

    auto candidate_itr{stop_iter};

    advance_with_wrap(candidate_itr);

    /* loop through the list, with wrap, until either we return to the start or find an opening */
    while (candidate_itr != stop_iter)
    {
        const auto possible_exit_location{move(*candidate_itr, entry_facing)};

        if (!check_terrain_is_opaque(this_ent, game_state.the_map[possible_exit_location]))
        {
            /* do one more check, which is if a moveable block is present */
            /* we make the check within this branch as the collision finder can be expensive */
            const auto entitr{find_entity_collision(this_ent, possible_exit_location,
                                                    std::begin(game_state.entity_list),
                                                    std::end(game_state.entity_list))};

            if (entitr->identity != entity_type::MOVEABLE_BLOCK)
            {
                /* we've found a valid opening, so we'll take it. */
                return std::make_pair(entry_facing, *candidate_itr);
            }
        }

        advance_with_wrap(candidate_itr);
    }

    /* if we reach here, we have returned to the beginning... spin the entity around */
    return std::make_pair(reverse(entry_facing), entry_location);
}

} // namespace chippie