#include "blob.hpp"
#include "chippie/chippie_common.hpp"
#include "chippie/entities/entity_types.hpp"
#include "chippie/state/terrain_effects.hpp"

#include "common/rng.hpp"

namespace chippie::blob
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

constexpr uint64_t BLOB_VELOCITY_US{1'000'000}; /* time is in us */

[[nodiscard]] std::pair<Grid::Location, direction> compute_next_location(const entity &ent) noexcept
{
    /* simply picks a random direction to move */
    const uint32_t random_value{rng::prng()};
    const auto new_facing{static_cast<direction>(static_cast<uint8_t>(direction::UP) + (random_value & 0b11))};
    return std::make_pair(move(ent.loc, new_facing), new_facing);
}

[[nodiscard]] collision_action handle_collision(entity &ent, entity *collided_entity,
                                                terrain_type collided_terrain) noexcept
{
    /* in the case of the purple ball:
            if it's clear, allow the move,
            if it's the player, game over,
            otherwise, reverse facing and move again
    */
    if (collided_entity != nullptr)
    {
        if (collided_entity->identity == entity_type::CHIPPIE)
        {
            register_event(event::event_type::ROLLED_BY_BALL);
        }
        return collision_action::NO_ACTION_NEEDED;
    }

    if (!check_terrain_is_opaque(ent, collided_terrain))
    {
        return collision_action::MOVE_FORWARD;
    }
    else
    {
        return collision_action::NO_ACTION_NEEDED;
    }
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
        .identity = entity_type::BLOB,
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
    return BLOB_VELOCITY_US;
}

} // namespace chippie::blob