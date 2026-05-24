#include "terrain_effects.hpp"

#include "chippie/chippie_common.hpp"
#include "chippie/entities/basic_entity.hpp"
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

inline void apply_fire_effect(entity &ent) noexcept
{
    if (check_is_chip(ent) && 0 == access_chippie_inventory().check(inventory_item::FIRE_BOOTS))
    {
        event::register_event(event::event_type::GAME_OVER);
    }
}

inline void apply_water_effect(entity &ent) noexcept
{
    if (check_is_chip(ent) && 0 == access_chippie_inventory().check(inventory_item::FLIPPERS))
    {
        event::register_event(event::event_type::GAME_OVER);
    }
}

inline void apply_ice_effect(entity &ent) noexcept
{
    if (!check_is_chip(ent) || 0 == access_chippie_inventory().check(inventory_item::ICE_SKATES))
    {
        move_relative_direction(ent, relative_direction::FORWARD);
        return;
    }
}

} // namespace

void apply_terrain_entry_effect(entity &ent, terrain_type &terrain) noexcept
{
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
    case terrain_type::ICE:
        apply_ice_effect(ent);
        break;
    case terrain_type::TRAP:
        ent.trapped = true;
        break;
    case terrain_type::CHIP:
        access_chippie_inventory().remove(inventory_item::CHIPS);
        terrain = terrain_type::CLEAR;
        break;
    case terrain_type::SOCKET:
    case terrain_type::GREEN_DOOR:
    case terrain_type::RED_DOOR:
    case terrain_type::CYAN_DOOR:
    case terrain_type::YELLOW_DOOR:
        terrain = terrain_type::CLEAR;
        break;
    case terrain_type::GREEN_KEY:
        access_chippie_inventory().add(inventory_item::GREEN_KEY);
        terrain = terrain_type::CLEAR;
        break;
    case terrain_type::RED_KEY:
        access_chippie_inventory().add(inventory_item::RED_KEY);
        terrain = terrain_type::CLEAR;
        break;
    case terrain_type::CYAN_KEY:
        access_chippie_inventory().add(inventory_item::CYAN_KEY);
        terrain = terrain_type::CLEAR;
        break;
    case terrain_type::YELLOW_KEY:
        access_chippie_inventory().add(inventory_item::YELLOW_KEY);
        terrain = terrain_type::CLEAR;
        break;
    case terrain_type::BOMB:
        ent.alive = false;
        terrain = terrain_type::CLEAR;
        break;
    case terrain_type::BROWN_BUTTON:
        event::register_event(event::event_type::RELEASE_ALL_TRAPS);
        break;

    case terrain_type::CLEAR:
    case terrain_type::WALL:
    case terrain_type::HINT:

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

void apply_terrain_exit_effect(entity &ent, terrain_type terrain) noexcept
{
}

[[nodiscard]] bool check_terrain_is_opaque(entity &ent, terrain_type terrain) noexcept
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

} // namespace chippie