#if !defined(TERRAIN_TYPES_HPP)
#define TERRAIN_TYPES_HPP

#include <cstdint>

namespace chippie
{

/** terrain types are anything that cannot move.
    This includes walls, invisible walls, appearing walls, ice, chips, doors, keys, and others.
 */

/* clang-format off */
enum struct terrain_type : uint8_t
{
    CLEAR  = 0b0000'0000,
    PORTAL = 0b0000'0001,
    WALL   = 0b0000'0010,
    CHIP   = 0b0000'0011,
    HINT   = 0b0000'0100,
    SOCKET = 0b0000'0101,

    /* non-directional */
    WATER  = 0b1000'0000,
    FIRE   = 0b1000'0001,
    GRAVEL = 0b1000'0010,
    ICE    = 0b1000'0011,
    DIRT   = 0b1000'0100,
    TRAP   = 0b1000'0101,
    BOMB   = 0b1000'0110,

    /* buttons, doors, keys */
    GREEN_BUTTON = 0b1100'0000,
    BLUE_BUTTON  = 0b1100'0001,
    BROWN_BUTTON = 0b1100'0010,
    RED_BUTTON   = 0b1100'0011,
    GREEN_DOOR   = 0b1100'0100,
    RED_DOOR     = 0b1100'0101,
    CYAN_DOOR    = 0b1100'0110,
    YELLOW_DOOR  = 0b1100'0111,
    GREEN_KEY    = 0b1100'1000,
    RED_KEY      = 0b1100'1001,
    CYAN_KEY     = 0b1100'1010,
    YELLOW_KEY   = 0b1100'1011,

    /* directional terrain features */
    ICE_TOPLEFT      = 0b0001'0000,
    ICE_TOPRIGHT     = 0b0001'0001,
    ICE_BOTLEFT      = 0b0001'0010,
    ICE_BOTRIGHT     = 0b0001'0011,
    PUSH_FLOOR_UP    = 0b0001'0100,
    PUSH_FLOOR_DOWN  = 0b0001'0101,
    PUSH_FLOOR_LEFT  = 0b0001'0110,
    PUSH_FLOOR_RIGHT = 0b0001'0111,
    THIN_WALL_TOP    = 0b0001'1000,
    THIN_WALL_BOT    = 0b0001'1001,
    THIN_WALL_LEFT   = 0b0001'1010,
    THIN_WALL_RIGHT  = 0b0001'1011,
};
/* clang-format on */
} // namespace chippie

#endif