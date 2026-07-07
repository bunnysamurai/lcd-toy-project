#include "revenge/revenge_defs.hpp"
#if !defined(TERRAIN_TYPES_HPP)
#define TERRAIN_TYPES_HPP

#include <cstdint>

namespace chippie
{

/** terrain types are anything that cannot move.
    This includes walls, invisible walls, appearing walls, ice, chips, doors, keys, and others.

    We also reserve the top bit to indicate if there is a movable block on top of the terrain.
    We leave it up to the static_map class to handle interpreting that top bit
 */

/* clang-format off */
enum struct terrain_type : uint8_t
{
    CLEAR  = 0b0000'0000,
    
    PORTAL,
    WALL,
    CHIP,
    HINT,
    SOCKET,

    /* non-directional */
    WATER,
    FIRE,
    GRAVEL,
    ICE,
    DIRT,
    TRAP,
    BOMB,
    INVISIBLE_WALL,
    APPEARING_WALL,
    MAGIC_TILE_WALL,
    MAGIC_TILE_CLEAR,

    /* buttons, doors, keys, clone machines, etc. */
    GREEN_BUTTON_CLEAR,
    GREEN_BUTTON_WALL,
    GREEN_BUTTON,
    BLUE_BUTTON,
    BROWN_BUTTON,
    RED_BUTTON,
    GREEN_DOOR,
    RED_DOOR,
    CYAN_DOOR,
    YELLOW_DOOR,
    GREEN_KEY,
    RED_KEY,
    CYAN_KEY,
    YELLOW_KEY,
    CLONER_FIRE_DANCER,

    /* directional terrain features */
    ICE_TOPLEFT,
    ICE_TOPRIGHT,
    ICE_BOTLEFT,
    ICE_BOTRIGHT,
    PUSH_FLOOR_UP,
    PUSH_FLOOR_DOWN,
    PUSH_FLOOR_LEFT,
    PUSH_FLOOR_RIGHT,
    THIN_WALL_TOP,
    THIN_WALL_BOT,
    THIN_WALL_LEFT,
    THIN_WALL_RIGHT,

    /* the loot! (boot) */
    FIRE_BOOTS,
    FLIPPERS,
    ICE_SKATES,
    SUCTION_BOOTS,
};
/* clang-format on */

} // namespace chippie

#endif