#include "terrain_effects.hpp"

#include "chippie/events/event.hpp"
#include "chippie/state/terrain_types.hpp"
#include "chippie_inventory.hpp"
#include "events/event_types.hpp"
#include "state.hpp"
#include "static_map.hpp"

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
        register_event(event_type::GAME_OVER);
    }
}

inline void apply_water_effect(entity &ent) noexcept
{
    if (check_is_chip(ent) && 0 == access_chippie_inventory().check(inventory_item::FLIPPERS))
    {
        register_event(event_type::GAME_OVER);
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

inline bool apply_chip_effect(entity &ent) noexcept
{
    if (check_is_chip(ent))
    {
    }
}

inline bool apply_socket_effect(entity &ent) noexcept
{
    return ent.identity == entity_type::CHIPPIE;
}

} // namespace

void apply_terrain_effect(entity &ent, terrain_type &terrain) noexcept
{
    switch (terrain)
    {
    case terrain_type::PORTAL:
        register_event(event_type::NEXT_LEVEL);
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
        get_game_state().decrement_chip_count();
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
        event::register_event(event_type::RELEASE_ALL_TRAPS);
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
    }
}
} // namespace chippie