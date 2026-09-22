#include "water_glider.hpp"
#include "chippie/chippie_common.hpp"
#include "chippie/entities/entity_types.hpp"
#include "chippie/state/terrain_effects.hpp"

namespace chippie::water_glider
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

constexpr uint64_t WATER_GLIDER_VELOCITY_US{250'000}; /* time is in us */

[[nodiscard]] bool check_tile_is_already_occupied(const entity &ent, const Grid::Location loc) noexcept
{
    const auto &the_list{ent.game_state->entity_list};
    const auto itr{find_entity_collision(ent, loc, std::begin(the_list), std::end(the_list))};

    return itr != std::end(the_list) && itr->identity != entity_type::CHIPPIE;
}

[[nodiscard]] std::pair<Grid::Location, direction> compute_next_location(const entity &ent) noexcept
{
    static constexpr std::array directions{
        relative_direction::FORWARD,
        relative_direction::LEFT,
        relative_direction::RIGHT,
        relative_direction::BACKWARD,
    };
    /*
        fire dancer movement logic is "anti-clockwise if colliding"
        It always wants to move in the following priority order
            forward
            left
            backward
            right

        TODO this logic is common among multiple entities, only differing by
        the order of relative directions in the `directions` above.
        Consider moving to a reusable function.
    */
    for (const auto dir : directions)
    {
        const auto [candidate_loc, candidate_facing]{move(ent.loc, ent.facing, dir)};

        const auto candidate_terrain{ent.game_state->the_map[candidate_loc]};

        if (check_terrain_is_opaque(ent.identity, candidate_facing, candidate_terrain) || check_tile_is_already_occupied(ent, candidate_loc))
        {
            continue;
        }

        return std::make_pair(candidate_loc, candidate_facing);
    }

    return std::make_pair(move(ent.loc, ent.facing), ent.facing);
}

[[nodiscard]] collision_action handle_collision(entity &ent, entity *collided_entity,
                                                terrain_type collided_terrain) noexcept
{
    if (collided_entity != nullptr)
    {
        if (collided_entity->identity == entity_type::CHIPPIE)
        {
            register_event(event::event_type::DANCED_BY_FIRE);
        }
        return collision_action::NO_ACTION_NEEDED;
    }

    if (check_terrain_is_opaque(ent.identity, ent.facing, collided_terrain))
    {
        return collision_action::NO_ACTION_NEEDED;
    }

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
[[nodiscard]] entity create(state &game_state, Grid::Location xy, direction dir, uint8_t uuid,
                            uint64_t next_time) noexcept
{
    return {
        .loc = xy,
        .identity = entity_type::WATER_GLIDER,
        .facing = dir,
        .next_time = next_time,
        .alive = true,
        .trapped = false,
        .uuid = uuid,
        .game_state = &game_state,
    };
}

entity_state_machine get_state_functions() noexcept
{
    return {
        .entry_handler = enforce_superposition_principle,
        .process_move_handler = compute_next_location,
        .entity_collision_handler = handle_collision,
        .exit_handler = nullptr,
    };
}

[[nodiscard]] uint64_t get_velocity() noexcept
{
    return WATER_GLIDER_VELOCITY_US;
}

} // namespace chippie::water_glider