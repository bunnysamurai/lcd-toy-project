#if !defined(BASIC_ENTITY_HPP)
#define BASIC_ENTITY_HPP

#include "chippie/chippie_common.hpp"
#include "chippie/state/terrain_types.hpp"
#include "utils/Grid.hpp"
#include "entity_types.hpp"

#include <cstdint>
#include <utility>

namespace chippie
{

struct state;

/** entities process via state machine?
    on every game loop iteration, the machine processes as follows:
    start-of-loop
    decide-next-location
    process-other-entity-collision
    process-terrain-type-to-be-entered
    end-of-loop

 */

/** entities are anything in the game that can "move"
    This includes purple balls, frog monsters, and bacteria, among others.
 */
struct entity
{
    Grid::Location loc;
    entity_type identity;
    direction facing;
    uint64_t next_time; /* useful for knowning when to processing moving */
    bool alive{true};
    bool trapped{false};
    uint8_t uuid;
    state *game_state;
};

enum struct collision_action
{
    NO_ACTION_NEEDED,
    APPLY_NEXT_LOCATION,
    MOVE_FORWARD,
    MOVE_BACKWARD,
    MOVE_RIGHT,
    MOVE_LEFT,
    MARK_DEAD
};

struct collision_result
{
    entity *other;
    terrain_type tile;
};

using entity_manip_fn = void (*)(entity &) noexcept;
using process_move_fn = std::pair<Grid::Location, direction> (*)(const entity &) noexcept;
using handle_collision_fn = collision_action (*)(entity &this_entity, entity *collided_entity,
                                                 terrain_type collided_terrain) noexcept;

struct entity_state_machine
{
    entity_manip_fn entry_handler;
    process_move_fn process_move_handler;
    handle_collision_fn entity_collision_handler;
    entity_manip_fn exit_handler;
};

using compute_next_fun_t = Grid::Location (*)(const entity &) noexcept;
using handle_collision_fun_t = collision_action (*)(entity &this_entity, entity *collided_entity,
                                                    terrain_type &collided_terrain) noexcept;

constexpr void move_relative_direction(entity &ent, relative_direction dir) noexcept
{
    const auto [newloc, newfac]{move(ent.loc, ent.facing, dir)};
    ent.loc = newloc;
    ent.facing = newfac;
}

[[nodiscard]] constexpr collision_action to_collision_action(relative_direction dir) noexcept
{
    switch (dir)
    {
    case relative_direction::BACKWARD:
        return collision_action::MOVE_BACKWARD;
    case relative_direction::FORWARD:
        return collision_action::MOVE_FORWARD;
    case relative_direction::LEFT:
        return collision_action::MOVE_LEFT;
    case relative_direction::RIGHT:
        return collision_action::MOVE_RIGHT;
    }
    return collision_action::NO_ACTION_NEEDED;
}

/**
    @brief Retrieve entity-specific functions for state machine processing.
 */
[[nodiscard]] entity_state_machine get_state_functions(entity_type id) noexcept;

/**
    @brief Retrieve entity-specific velocity for next-move.
 */
[[nodiscard]] uint64_t get_entity_velocity(entity_type id) noexcept;

} // namespace chippie

#endif