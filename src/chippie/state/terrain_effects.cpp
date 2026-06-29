#include "terrain_effects.hpp"

#include "chippie/chippie_common.hpp"
#include "chippie/entities/basic_entity.hpp"
#include "chippie/entities/entity_types.hpp"
#include "chippie/events/event.hpp"
#include "chippie/events/event_types.hpp"
#include "chippie/state/terrain_types.hpp"
#include "chippie_inventory.hpp"

namespace chippie
{
namespace
{
inline constexpr bool check_is_chip(const entity &ent) noexcept
{
    return ent.identity == entity_type::CHIPPIE;
}

inline constexpr bool check_has_item(const entity &ent, inventory_item item) noexcept
{
    return ent.game_state->chippie_inventory.check(item) > 0;
}

inline constexpr bool check_for_chip_and_item(const entity &ent, inventory_item item) noexcept
{
    return check_is_chip(ent) && check_has_item(ent, item);
};

inline void apply_fire_effect(entity &ent) noexcept
{
    if (check_is_chip(ent) && 0 == ent.game_state->chippie_inventory.check(inventory_item::FIRE_BOOTS))
    {
        event::register_event(event::event_type::GOT_BURNED);
    }
}

inline void apply_water_effect(entity &ent) noexcept
{
    if (check_is_chip(ent) && 0 == ent.game_state->chippie_inventory.check(inventory_item::FLIPPERS))
    {
        event::register_event(event::event_type::FELL_IN_WATER);
        return;
    }

    if (ent.identity == entity_type::MOVEABLE_BLOCK)
    {
        ent.game_state->the_map[ent.loc] = terrain_type::DIRT;
        ent.alive = false;
    }
}

} // namespace

void apply_terrain_entry_effect(entity &ent, terrain_type terrain) noexcept
{
    auto &map_tile{ent.game_state->the_map[ent.loc]};

    switch (terrain)
    {
    case terrain_type::PORTAL:
        event::register_event(event::event_type::NEXT_LEVEL);
        break;
    case terrain_type::FIRE:
        apply_fire_effect(ent);
        break;
    case terrain_type::WATER:
        apply_water_effect(ent);
        break;
    case terrain_type::TRAP:
        ent.trapped = true;
        break;
    case terrain_type::CHIP:
        ent.game_state->chippie_inventory.remove(inventory_item::CHIPS);
        map_tile = terrain_type::CLEAR;
        break;
    case terrain_type::SOCKET:
    case terrain_type::GREEN_DOOR:
    case terrain_type::RED_DOOR:
    case terrain_type::CYAN_DOOR:
    case terrain_type::YELLOW_DOOR:
        map_tile = terrain_type::CLEAR;
        break;
    case terrain_type::GREEN_KEY:
        ent.game_state->chippie_inventory.add(inventory_item::GREEN_KEY);
        map_tile = terrain_type::CLEAR;
        break;
    case terrain_type::RED_KEY:
        ent.game_state->chippie_inventory.add(inventory_item::RED_KEY);
        map_tile = terrain_type::CLEAR;
        break;
    case terrain_type::CYAN_KEY:
        ent.game_state->chippie_inventory.add(inventory_item::CYAN_KEY);
        map_tile = terrain_type::CLEAR;
        break;
    case terrain_type::YELLOW_KEY:
        ent.game_state->chippie_inventory.add(inventory_item::YELLOW_KEY);
        map_tile = terrain_type::CLEAR;
        break;
    case terrain_type::BOMB:
        ent.alive = false;
        map_tile = terrain_type::CLEAR;
        break;
    case terrain_type::BROWN_BUTTON:
        event::register_event(event::event_type::RELEASE_ALL_TRAPS);
        break;

    case terrain_type::HINT:
        event::register_event(event::event_type::DISPLAY_HINT);
        break;

    case terrain_type::DIRT:
        map_tile = terrain_type::CLEAR;
        break;

    /* directional terrain features */
    case terrain_type::ICE_TOPLEFT:
        if (!check_for_chip_and_item(ent, inventory_item::ICE_SKATES))
        {
            ent.facing = ent.facing == direction::LEFT ? direction::DOWN : direction::RIGHT;
        }
        break;
    case terrain_type::ICE_TOPRIGHT:
        if (!check_for_chip_and_item(ent, inventory_item::ICE_SKATES))
        {
            ent.facing = ent.facing == direction::RIGHT ? direction::DOWN : direction::LEFT;
        }
        break;
    case terrain_type::ICE_BOTLEFT:
        if (!check_for_chip_and_item(ent, inventory_item::ICE_SKATES))
        {
            ent.facing = ent.facing == direction::LEFT ? direction::UP : direction::RIGHT;
        }
        break;
    case terrain_type::ICE_BOTRIGHT:
        if (!check_for_chip_and_item(ent, inventory_item::ICE_SKATES))
        {
            ent.facing = ent.facing == direction::RIGHT ? direction::UP : direction::LEFT;
        }
        break;
    case terrain_type::PUSH_FLOOR_UP:
        if (!check_for_chip_and_item(ent, inventory_item::SUCTION_BOOTS))
        {
            ent.facing = direction::UP;
        }
        break;
    case terrain_type::PUSH_FLOOR_DOWN:
        if (!check_for_chip_and_item(ent, inventory_item::SUCTION_BOOTS))
        {
            ent.facing = direction::DOWN;
        }
        break;
    case terrain_type::PUSH_FLOOR_LEFT:
        if (!check_for_chip_and_item(ent, inventory_item::SUCTION_BOOTS))
        {
            ent.facing = direction::LEFT;
        }
        break;
    case terrain_type::PUSH_FLOOR_RIGHT:
        if (!check_for_chip_and_item(ent, inventory_item::SUCTION_BOOTS))
        {
            ent.facing = direction::RIGHT;
        }
        break;

    /* the loot! (boots) */
    case terrain_type::FIRE_BOOTS:
        ent.game_state->chippie_inventory.add(inventory_item::FIRE_BOOTS);
        map_tile = terrain_type::CLEAR;
        break;
    case terrain_type::FLIPPERS:
        ent.game_state->chippie_inventory.add(inventory_item::FLIPPERS);
        map_tile = terrain_type::CLEAR;
        break;
    case terrain_type::ICE_SKATES:
        ent.game_state->chippie_inventory.add(inventory_item::ICE_SKATES);
        map_tile = terrain_type::CLEAR;
        break;
    case terrain_type::SUCTION_BOOTS:
        ent.game_state->chippie_inventory.add(inventory_item::SUCTION_BOOTS);
        map_tile = terrain_type::CLEAR;
        break;

    case terrain_type::ICE:
    case terrain_type::GREEN_BUTTON:
    case terrain_type::BLUE_BUTTON:
    case terrain_type::RED_BUTTON:
    case terrain_type::CLEAR:
    case terrain_type::WALL:
    case terrain_type::GRAVEL:
    case terrain_type::THIN_WALL_TOP:
    case terrain_type::THIN_WALL_BOT:
    case terrain_type::THIN_WALL_LEFT:
    case terrain_type::THIN_WALL_RIGHT:
        break;
    }
}

void apply_terrain_exit_effect(entity &ent, terrain_type terrain) noexcept
{
    switch (terrain)
    {
    case terrain_type::HINT:
        event::register_event(event::event_type::CLEAR_HINT);
        break;
    case terrain_type::PORTAL:
    case terrain_type::FIRE:
    case terrain_type::WATER:
    case terrain_type::ICE:
    case terrain_type::TRAP:
    case terrain_type::CHIP:
    case terrain_type::SOCKET:
    case terrain_type::GREEN_DOOR:
    case terrain_type::RED_DOOR:
    case terrain_type::CYAN_DOOR:
    case terrain_type::YELLOW_DOOR:
    case terrain_type::GREEN_KEY:
    case terrain_type::RED_KEY:
    case terrain_type::CYAN_KEY:
    case terrain_type::YELLOW_KEY:
    case terrain_type::BOMB:
    case terrain_type::BROWN_BUTTON:
    case terrain_type::CLEAR:
    case terrain_type::WALL:

    /* non-directional */
    case terrain_type::GRAVEL:
    case terrain_type::DIRT:

    /* buttons, doors, keys */
    case terrain_type::GREEN_BUTTON:
    case terrain_type::BLUE_BUTTON:
    case terrain_type::RED_BUTTON:

    /* directional terrain features */
    case terrain_type::ICE_TOPLEFT:
    case terrain_type::ICE_TOPRIGHT:
    case terrain_type::ICE_BOTLEFT:
    case terrain_type::ICE_BOTRIGHT:
    case terrain_type::PUSH_FLOOR_UP:
    case terrain_type::PUSH_FLOOR_DOWN:
    case terrain_type::PUSH_FLOOR_LEFT:
    case terrain_type::PUSH_FLOOR_RIGHT:
    case terrain_type::THIN_WALL_TOP:
    case terrain_type::THIN_WALL_BOT:
    case terrain_type::THIN_WALL_LEFT:
    case terrain_type::THIN_WALL_RIGHT:
        break;
    }
}

void apply_terrain_override_effect(entity &ent, Grid::Location &nextloc, direction &nextfacing,
                                   terrain_type terrain) noexcept
{
    static constexpr uint64_t FIXED_PERIOD_US{100'000};

    auto &&to_direction{[&](terrain_type push_floor_typespecific_type) {
        if (push_floor_typespecific_type == terrain_type::PUSH_FLOOR_DOWN)
        {
            return direction::DOWN;
        }
        if (push_floor_typespecific_type == terrain_type::PUSH_FLOOR_UP)
        {
            return direction::UP;
        }
        if (push_floor_typespecific_type == terrain_type::PUSH_FLOOR_LEFT)
        {
            return direction::LEFT;
        }
        if (push_floor_typespecific_type == terrain_type::PUSH_FLOOR_RIGHT)
        {
            return direction::RIGHT;
        }

        return ent.facing;
    }};

    /* just the force floors and ice do this */
    switch (terrain)
    {
    case terrain_type::ICE:
    case terrain_type::ICE_TOPLEFT:
    case terrain_type::ICE_TOPRIGHT:
    case terrain_type::ICE_BOTLEFT:
    case terrain_type::ICE_BOTRIGHT:
        if (!check_for_chip_and_item(ent, inventory_item::ICE_SKATES))
        {
            nextloc = move(ent.loc, ent.facing);
            nextfacing = ent.facing;
            ent.next_time = ent.next_time - get_entity_velocity(ent.identity) + FIXED_PERIOD_US;
        }
        break;
    /* push floors allow movements if it's orthogonal to the floor's direction */
    /* so for example, with PUSH_FLOOR_UP, if the next facing is either LEFT or RIGHT, do nothing */
    /* otherwise, behaviour is identical to ice */
    case terrain_type::PUSH_FLOOR_UP:
    case terrain_type::PUSH_FLOOR_DOWN:
        if (!(check_for_chip_and_item(ent, inventory_item::SUCTION_BOOTS) ||
              (ent.facing != direction::LEFT && nextfacing == direction::LEFT) ||
              (ent.facing != direction::RIGHT && nextfacing == direction::RIGHT)))
        {
            nextloc = move(ent.loc, ent.facing);
            nextfacing = to_direction(terrain);
            ent.next_time = ent.next_time - get_entity_velocity(ent.identity) + FIXED_PERIOD_US;
        }
        break;
    case terrain_type::PUSH_FLOOR_LEFT:
    case terrain_type::PUSH_FLOOR_RIGHT:
        if (!(check_for_chip_and_item(ent, inventory_item::SUCTION_BOOTS) ||
              (ent.facing != direction::UP && nextfacing == direction::UP) ||
              (ent.facing != direction::DOWN && nextfacing == direction::DOWN)))
        {
            nextloc = move(ent.loc, ent.facing);
            nextfacing = to_direction(terrain);
            ent.next_time = ent.next_time - get_entity_velocity(ent.identity) + FIXED_PERIOD_US;
        }
        break;

    default:
        break;
    }
}

[[nodiscard]] bool check_terrain_is_opaque(const entity &ent, terrain_type terrain) noexcept
{
    switch (terrain)
    {
    case terrain_type::WALL:
    case terrain_type::SOCKET:
    case terrain_type::CHIP:
    case terrain_type::GRAVEL:
    case terrain_type::DIRT:
    case terrain_type::GREEN_DOOR:
    case terrain_type::RED_DOOR:
    case terrain_type::CYAN_DOOR:
    case terrain_type::YELLOW_DOOR:
        return true;

    case terrain_type::THIN_WALL_TOP:
        return ent.facing == direction::DOWN;
    case terrain_type::THIN_WALL_BOT:
        return ent.facing == direction::UP;
    case terrain_type::THIN_WALL_LEFT:
        return ent.facing == direction::RIGHT;
    case terrain_type::THIN_WALL_RIGHT:
        return ent.facing == direction::LEFT;

    case terrain_type::ICE_TOPLEFT:
        return ent.facing == direction::DOWN || ent.facing == direction::RIGHT;
    case terrain_type::ICE_TOPRIGHT:
        return ent.facing == direction::DOWN || ent.facing == direction::LEFT;
    case terrain_type::ICE_BOTLEFT:
        return ent.facing == direction::UP || ent.facing == direction::RIGHT;
    case terrain_type::ICE_BOTRIGHT:
        return ent.facing == direction::UP || ent.facing == direction::LEFT;

    case terrain_type::CLEAR:
    case terrain_type::PORTAL:
    case terrain_type::HINT:
    case terrain_type::WATER:
    case terrain_type::FIRE:
    case terrain_type::ICE:
    case terrain_type::TRAP:
    case terrain_type::BOMB:
    case terrain_type::GREEN_BUTTON:
    case terrain_type::BLUE_BUTTON:
    case terrain_type::BROWN_BUTTON:
    case terrain_type::RED_BUTTON:
    case terrain_type::GREEN_KEY:
    case terrain_type::RED_KEY:
    case terrain_type::CYAN_KEY:
    case terrain_type::YELLOW_KEY:
    case terrain_type::PUSH_FLOOR_UP:
    case terrain_type::PUSH_FLOOR_DOWN:
    case terrain_type::PUSH_FLOOR_LEFT:
    case terrain_type::PUSH_FLOOR_RIGHT:
        return false;
    }

    return false;
}

[[nodiscard]] bool check_terrain_is_opaque_for_moveable(direction dir, terrain_type terrain) noexcept
{
    switch (terrain)
    {
    case terrain_type::WALL:
    case terrain_type::DIRT:
    case terrain_type::PORTAL:
    case terrain_type::SOCKET:
    case terrain_type::GRAVEL:
    case terrain_type::GREEN_DOOR:
    case terrain_type::RED_DOOR:
    case terrain_type::CYAN_DOOR:
    case terrain_type::YELLOW_DOOR:
        return true;

    case terrain_type::THIN_WALL_TOP:
        return dir == direction::DOWN;
    case terrain_type::THIN_WALL_BOT:
        return dir == direction::UP;
    case terrain_type::THIN_WALL_LEFT:
        return dir == direction::RIGHT;
    case terrain_type::THIN_WALL_RIGHT:
        return dir == direction::LEFT;

    case terrain_type::ICE_TOPLEFT:
        return dir == direction::DOWN || dir == direction::RIGHT;
    case terrain_type::ICE_TOPRIGHT:
        return dir == direction::DOWN || dir == direction::LEFT;
    case terrain_type::ICE_BOTLEFT:
        return dir == direction::UP || dir == direction::RIGHT;
    case terrain_type::ICE_BOTRIGHT:
        return dir == direction::UP || dir == direction::LEFT;

    case terrain_type::CLEAR:
    case terrain_type::CHIP:
    case terrain_type::HINT:
    case terrain_type::WATER:
    case terrain_type::FIRE:
    case terrain_type::ICE:
    case terrain_type::TRAP:
    case terrain_type::BOMB:
    case terrain_type::GREEN_BUTTON:
    case terrain_type::BLUE_BUTTON:
    case terrain_type::BROWN_BUTTON:
    case terrain_type::RED_BUTTON:
    case terrain_type::GREEN_KEY:
    case terrain_type::RED_KEY:
    case terrain_type::CYAN_KEY:
    case terrain_type::YELLOW_KEY:
    case terrain_type::PUSH_FLOOR_UP:
    case terrain_type::PUSH_FLOOR_DOWN:
    case terrain_type::PUSH_FLOOR_LEFT:
    case terrain_type::PUSH_FLOOR_RIGHT:
        return false;
    }

    return false;
}

} // namespace chippie