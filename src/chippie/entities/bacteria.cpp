#include "bacteria.hpp"
#include "chippie/chippie_common.hpp"
#include "chippie/entities/entity_types.hpp"
#include "chippie/state/terrain_effects.hpp"

#include <array>

#include <algorithm>

#define DEBUG_PRINT
#ifdef DEBUG_PRINT
#include "pico/printf.h"
#endif

namespace chippie::bacteria
{

/*
 ____       _            _
|  _ \ _ __(_)_   ____ _| |_ ___
| |_) | '__| \ \ / / _` | __/ _ \
|  __/| |  | |\ V / (_| | ||  __/
|_|   |_|  |_| \_/ \__,_|\__\___|

*/
namespace
{

constexpr std::array directions{
    relative_direction::RIGHT,
    relative_direction::FORWARD,
    relative_direction::LEFT,
    relative_direction::BACKWARD,
};

constexpr uint64_t BACTERIA_VELOCITY_US{250'000}; /* time is in us */

[[nodiscard]] bool check_terrain_is_opaque_for_bacteria(const entity &ent, terrain_type candidate_terrain) noexcept
{
    return check_terrain_is_opaque(ent, candidate_terrain) || candidate_terrain == terrain_type::FIRE;
}

[[nodiscard]] std::pair<Grid::Location, direction> compute_next_location(const entity &ent) noexcept
{
    /*
        bacteria movement logic is "clockwise"
        It always wants to move to the following relative directions, in order:
            left
            forward
            right
            backward
    */
#ifdef DEBUG_PRINT
    printf("bacteria moving start\n");
#endif
    for (const auto dir : directions)
    {
        const auto [candidate_loc, candidate_facing]{move(ent.loc, ent.facing, dir)};

        const auto candidate_terrain{ent.game_state->the_map[candidate_loc]};

        /* check if */
        if (check_terrain_is_opaque_for_bacteria(ent, candidate_terrain))
        {
            continue;
        }

        return std::make_pair(candidate_loc, candidate_facing);
    }

#ifdef DEBUG_PRINT
    printf("bacteria not moving result.. end\n");
#endif
    return std::make_pair(ent.loc, ent.facing);
}

[[nodiscard]] collision_action handle_collision(entity &ent, entity *collided_entity,
                                                terrain_type collided_terrain) noexcept
{
#ifdef DEBUG_PRINT
    printf("bacteria collision handling start\n");
#endif
    if (collided_entity != nullptr)
    {
        if (collided_entity->identity == entity_type::CHIPPIE)
        {
#ifdef DEBUG_PRINT
            printf("collided with chip\n");
#endif
            register_event(event::event_type::EATEN_BY_BUG);
        }
        else
        {
            /* rerun the same logic */
            for (const auto dir : directions)
            {
                const auto [candidate_loc, candidate_facing]{move(ent.loc, ent.facing, dir)};

                const auto candidate_terrain{ent.game_state->the_map[candidate_loc]};

                /* check if */
                if (check_terrain_is_opaque_for_bacteria(ent, candidate_terrain))
                {
                    continue;
                }

                /* search for other entities */
                auto entitr = std::find_if(
                    std::begin(ent.game_state->entity_list), std::end(ent.game_state->entity_list),
                    [&](const auto &other) { return (other.loc == candidate_loc) && (ent.uuid != other.uuid); });

                if (entitr == std::end(ent.game_state->entity_list))
                {
                    return to_collision_action(dir);
                }
            }

            return collision_action::NO_ACTION_NEEDED;
        }
    }

#ifdef DEBUG_PRINT
    printf("bacteria collision handling end\n");
#endif

    return collision_action::APPLY_NEXT_LOCATION;
}

} // namespace

/*
 ____        _     _ _
|  _ \ _   _| |__ | (_) ___
| |_) | | | | '_ \| | |/ __|
|  __/| |_| | |_) | | | (__
|_|    \__,_|_.__/|_|_|\___|

*/
[[nodiscard]] entity create(state &game_state, Grid::Location xy, direction dir, uint8_t uuid) noexcept
{
    return {
        .loc = xy,
        .identity = entity_type::BACTERIA,
        .facing = dir,
        .alive = true,
        .trapped = false,
        .uuid = uuid,
        .game_state = &game_state,
    };
}

entity_state_machine get_state_functions() noexcept
{
    return {
        .entry_handler = nullptr,
        .process_move_handler = compute_next_location,
        .entity_collision_handler = handle_collision,
        .exit_handler = nullptr,
    };
}

[[nodiscard]] uint64_t get_velocity() noexcept
{
    return BACTERIA_VELOCITY_US;
}

} // namespace chippie::bacteria