#if !defined(BASIC_ENTITY_HPP)
#define BASIC_ENTITY_HPP

#include "chippie/chippie_common.hpp"
#include "chippie/state/terrain_types.hpp"
#include "common/Grid.hpp"
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

/* entity manipulating functions... leaving these free for now as that makes things less complicated.  I might be a C
 * programmer now :() */

[[nodiscard]] constexpr direction to_absolute_direction(direction facing, relative_direction dir) noexcept
{
    return direction::DOWN;
};

constexpr void move_relative_direction(entity &ent, relative_direction dir) noexcept
{
    const direction absolute{to_absolute_direction(ent.facing, dir)};
    ent.loc = move(ent.loc, absolute);
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