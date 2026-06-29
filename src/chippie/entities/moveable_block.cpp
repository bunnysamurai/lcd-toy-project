#include "moveable_block.hpp"
#include "chippie/chippie_common.hpp"
#include "chippie/entities/basic_entity.hpp"
#include "chippie/entities/entity_types.hpp"
#include "chippie/state/terrain_effects.hpp"

// #define DEBUG_PRINT
#ifdef DEBUG_PRINT
#include "pico/printf.h"
#endif

namespace chippie::moveable_block_entity
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

constexpr uint64_t MOVEABLE_BLOCK_VELOCITY_US{0}; /* time is in us */

[[nodiscard]] std::pair<Grid::Location, direction> move_if_chippie_is_on_us(const entity &ent) noexcept
{
    /* moveable block is a bit of a special snowflake, in that if chippie is on our location,
    then it is ALWAYS okay to move.  This is because by design part of the logic to move this block is contained in
    chippie_entity.cpp, a la, chippie will check if the space behind the block is clear to move.  */

    /* TODO by convention, chippie is always the first entity in the entity_list.. should probably write a test for
     * that. */

#ifdef DEBUG_PRINT
    printf("moveable block: processing a move...\n");
#endif
    const auto &chippie_as_entity{ent.game_state->entity_list.front()};

    if (ent.loc == chippie_as_entity.loc)
    {
#ifdef DEBUG_PRINT
        printf("moveable block: has chippie on top!\n");
#endif
        return std::make_pair(move(ent.loc, chippie_as_entity.facing), chippie_as_entity.facing);
    }

#ifdef DEBUG_PRINT
        printf("moveable block: no chippie on top...\n");
#endif
    return std::make_pair(ent.loc, ent.facing);
}

[[nodiscard]] collision_action always_apply_next_location([[maybe_unused]] entity &ent,
                                                          [[maybe_unused]] entity *collided_entity,
                                                          [[maybe_unused]] terrain_type collided_terrain) noexcept
{
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
        .identity = entity_type::MOVEABLE_BLOCK,
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
        .process_move_handler = move_if_chippie_is_on_us,
        .entity_collision_handler = always_apply_next_location,
        .exit_handler = nullptr,
    };
}

[[nodiscard]] uint64_t get_velocity() noexcept
{
    return MOVEABLE_BLOCK_VELOCITY_US;
}

} // namespace chippie::moveable_block_entity