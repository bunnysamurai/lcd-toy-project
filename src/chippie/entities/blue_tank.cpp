#include "blue_tank.hpp"
#include "chippie/chippie_common.hpp"
#include "chippie/entities/entity_types.hpp"
#include "chippie/state/terrain_effects.hpp"

#include <array>

#define DEBUG_PRINT
#ifdef DEBUG_PRINT
#include "pico/printf.h"
#endif

namespace chippie::blue_tank
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

constexpr uint64_t BLUE_TANK_VELOCITY_US{250'000}; /* time is in us */

[[nodiscard]] bool check_terrain_is_opaque_for_blue_tank(const entity &ent, terrain_type candidate_terrain) noexcept
{
    return check_terrain_is_opaque(ent, candidate_terrain);
}

[[nodiscard]] std::pair<Grid::Location, direction> compute_next_location(const entity &ent) noexcept
{
    static constexpr std::array clockwise_move_table{
        std::make_pair(entity_type::BLUE_TANK_THAT_MOVES_LEFT, direction::LEFT),
        std::make_pair(entity_type::BLUE_TANK_THAT_MOVES_UP, direction::UP),
        std::make_pair(entity_type::BLUE_TANK_THAT_MOVES_RIGHT, direction::RIGHT),
        std::make_pair(entity_type::BLUE_TANK_THAT_MOVES_DOWN, direction::DOWN),
    };

    /*
        blue_tank movement is
        all blue tank entity's have a preferred direction ( see the entity_type identifier for a clue ;) )
        if not facing the entity's preferred direction, rotate clockwise
        if facing the entity's preferred direction, move 1 tile
    */

    const auto preferred_direction{std::ranges::find_if(clockwise_move_table, [id = ent.identity](const auto &item) {
                                       return item.first == id;
                                   })->second};

    if (ent.facing != preferred_direction)
    {
        return std::make_pair(ent.loc, rotate_clockwise(ent.facing));
    }

    return std::make_pair(move(ent.loc, preferred_direction), preferred_direction);
}

[[nodiscard]] collision_action handle_collision(entity &ent, entity *collided_entity,
                                                terrain_type collided_terrain) noexcept
{
    /* in the case of the blue_tank:
            if it's clear, allow the move,
            if it's the player, game over,
            otherwise, reverse facing and move again
    */
#ifdef DEBUG_PRINT
    printf("blue_tank collision handling start\n");
#endif
    if (collided_entity != nullptr)
    {
        if (collided_entity->identity == entity_type::CHIPPIE)
        {
#ifdef DEBUG_PRINT
            printf("collided with chip\n");
#endif
            register_event(event::event_type::FLATTENED_BY_TANK);
        }
        else
        {
            return collision_action::NO_ACTION_NEEDED;
        }
    }

    if (check_terrain_is_opaque_for_blue_tank(ent, collided_terrain))
    {
        return collision_action::NO_ACTION_NEEDED;
    }

#ifdef DEBUG_PRINT
    printf("blue_tank collision handling end\n");
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
    auto &&to_identity{[](const direction entdir) {
        switch (entdir)
        {
        case direction::UP:
            return entity_type::BLUE_TANK_THAT_MOVES_UP;
        case direction::DOWN:
            return entity_type::BLUE_TANK_THAT_MOVES_DOWN;
        case direction::LEFT:
            return entity_type::BLUE_TANK_THAT_MOVES_LEFT;
        case direction::RIGHT:
            return entity_type::BLUE_TANK_THAT_MOVES_RIGHT;
        }
        return entity_type::BLUE_TANK_THAT_MOVES_UP;
    }};

    return {
        .loc = xy,
        .identity = to_identity(dir),
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
    return BLUE_TANK_VELOCITY_US;
}

} // namespace chippie::blue_tank