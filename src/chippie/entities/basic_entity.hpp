#if !defined(BASIC_ENTITY_HPP)
#define BASIC_ENTITY_HPP

#include "chippie/chippie_common.hpp"
#include "chippie/state/terrain_types.hpp"
#include "entity_types.hpp"
#include "utils/Grid.hpp"

#include <algorithm>
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

    See game_logic.cpp for the actual details of the processing loop.  The above may be out-of-date.
 */

/** entities are anything in the game that can "move"
    This includes purple balls, frog monsters, and bacteria, among others.
 */
struct entity
{
    Grid::Location loc;
    entity_type identity;
    direction facing;
    uint64_t next_time; /* useful for knowing when to processing moving */

    /* bitfield for various states */
    uint8_t alive : 1 {true};    /* if set to false, game logic will remove this entity from the game state */
    uint8_t trapped : 1 {false}; /* special handling for brown button uses */
    uint8_t has_summoning_sickness : 1 {false}; /* special handling just for red button uses */
    uint8_t reserved : 5;

    uint8_t uuid;
    state *game_state;
};

[[nodiscard]] entity create_entity(entity_type ent_id, Grid::Location location, direction facing, uint8_t uuid,
                                   state &game_state, uint64_t next_time = 0) noexcept;

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

/** @brief common catch-all for entity edge case.

    A common cleanup utility, this will make the entity as dead if, on entry,
    this entity occupies the same space as another.  Used for
    entity_state_machine's entry_handler, mostly.

    Does run an entire find_collision check, just to raise awareness.
 */
void enforce_superposition_principle(entity &myself) noexcept;

/**
    @brief Find an entity in the same location as another
 */
template <class Iter>
[[nodiscard]] constexpr Iter find_entity_collision(const entity &current_processing_ent,
                                                   Grid::Location location_to_test, Iter entity_list_begin,
                                                   Iter entity_list_end) noexcept
{
    return std::find_if(entity_list_begin, entity_list_end, [&](const auto &other) {
        return (other.loc == location_to_test) && (current_processing_ent.uuid != other.uuid);
    });
}

} // namespace chippie

#endif