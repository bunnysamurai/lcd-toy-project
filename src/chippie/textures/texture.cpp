#include "texture.hpp"

#include "chippie/entities/entity_types.hpp"

#include "tiles/chip.hpp"
#ifdef CHIPPIE_IS_GIRL
#include "tiles/chippie_down_girl.hpp"
#include "tiles/chippie_left_girl.hpp"
#include "tiles/chippie_right_girl.hpp"
#include "tiles/chippie_up_girl.hpp"
#else
#include "tiles/chippie_down.hpp"
#include "tiles/chippie_left.hpp"
#include "tiles/chippie_right.hpp"
#include "tiles/chippie_up.hpp"
#endif
#include "tiles/clear.hpp"
#include "tiles/cyan_door.hpp"
#include "tiles/cyan_key.hpp"
#include "tiles/green_door.hpp"
#include "tiles/green_key.hpp"
#include "tiles/hint.hpp"
#include "tiles/portal.hpp"
#include "tiles/red_door.hpp"
#include "tiles/red_key.hpp"
#include "tiles/socket.hpp"
#include "tiles/wall.hpp"
#include "tiles/yellow_door.hpp"
#include "tiles/yellow_key.hpp"

#include "tiles/digit_00.hpp"
#include "tiles/digit_01.hpp"
#include "tiles/digit_02.hpp"
#include "tiles/digit_03.hpp"
#include "tiles/digit_04.hpp"
#include "tiles/digit_05.hpp"
#include "tiles/digit_06.hpp"
#include "tiles/digit_07.hpp"
#include "tiles/digit_08.hpp"
#include "tiles/digit_09.hpp"

namespace chippie::texture
{

texture_type get_terrain_texture_type(chippie::terrain_type terrain) noexcept
{
    switch (terrain)
    {
    case terrain_type::CLEAR:
        return texture_type::CLEAR;
    case terrain_type::PORTAL:
        return texture_type::PORTAL;
    case terrain_type::WALL:
        return texture_type::WALL;
    case terrain_type::CHIP:
        return texture_type::CHIP;
    case terrain_type::HINT:
        return texture_type::HINT;
    case terrain_type::SOCKET:
        return texture_type::SOCKET;
    case terrain_type::GREEN_DOOR:
        return texture_type::GREEN_DOOR;
    case terrain_type::RED_DOOR:
        return texture_type::RED_DOOR;
    case terrain_type::CYAN_DOOR:
        return texture_type::CYAN_DOOR;
    case terrain_type::YELLOW_DOOR:
        return texture_type::YELLOW_DOOR;
    case terrain_type::GREEN_KEY:
        return texture_type::GREEN_KEY;
    case terrain_type::RED_KEY:
        return texture_type::RED_KEY;
    case terrain_type::CYAN_KEY:
        return texture_type::CYAN_KEY;
    case terrain_type::YELLOW_KEY:
        return texture_type::YELLOW_KEY;
    /* not yet implemented */
    case terrain_type::WATER:
    case terrain_type::FIRE:
    case terrain_type::GRAVEL:
    case terrain_type::ICE:
    case terrain_type::DIRT:
    case terrain_type::TRAP:
    case terrain_type::BOMB:
    case terrain_type::GREEN_BUTTON:
    case terrain_type::BLUE_BUTTON:
    case terrain_type::BROWN_BUTTON:
    case terrain_type::RED_BUTTON:
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
        return texture_type::CLEAR;
    }

    return texture_type::CLEAR;
}

texture_type get_entity_texture_type(const chippie::entity &ent) noexcept
{
    switch (ent.identity)
    {
    case entity_type::CHIPPIE:
        return static_cast<texture_type>(static_cast<int>(texture_type::CHIPPIE_UP) + static_cast<int>(ent.facing));
    case entity_type::BLUE_TANK:
    case entity_type::WATER_GLIDER:
    case entity_type::FIRE_DANCER:
    case entity_type::PURPLE_BALL:
    case entity_type::CYAN_STICK_BALL:
    case entity_type::BACTERIA:
    case entity_type::FROG_MONSTER:
    case entity_type::CENTIPEDE:
    case entity_type::ENTITY_TYPE_SIZE:
        return texture_type::CLEAR; /* TODO not yet implemented*/
    }

    return texture_type::CLEAR; /* The sane default? */
}

screen::Tile get_texture(texture_type texture) noexcept
{
    switch (texture)
    {
    case texture_type::CHIP:
        return chip::get_texture();
    case texture_type::CHIPPIE_DOWN:
#ifdef CHIPPIE_IS_GIRL
        return chippie_down_girl::get_texture();
#else
        return chippie_down::get_texture();
#endif
    case texture_type::CHIPPIE_LEFT:
#ifdef CHIPPIE_IS_GIRL
        return chippie_left_girl::get_texture();
#else
        return chippie_left::get_texture();
#endif
    case texture_type::CHIPPIE_RIGHT:
#ifdef CHIPPIE_IS_GIRL
        return chippie_right_girl::get_texture();
#else
        return chippie_right::get_texture();
#endif
    case texture_type::CHIPPIE_UP:
#ifdef CHIPPIE_IS_GIRL
        return chippie_up_girl::get_texture();
#else
        return chippie_up::get_texture();
#endif
    case texture_type::CLEAR:
        return clear::get_texture();
    case texture_type::CYAN_DOOR:
        return cyan_door::get_texture();
    case texture_type::CYAN_KEY:
        return cyan_key::get_texture();
    case texture_type::GREEN_DOOR:
        return green_door::get_texture();
    case texture_type::GREEN_KEY:
        return green_key::get_texture();
    case texture_type::HINT:
        return hint::get_texture();
    case texture_type::PORTAL:
        return portal::get_texture();
    case texture_type::RED_DOOR:
        return red_door::get_texture();
    case texture_type::RED_KEY:
        return red_key::get_texture();
    case texture_type::SOCKET:
        return socket::get_texture();
    case texture_type::WALL:
        return wall::get_texture();
    case texture_type::YELLOW_DOOR:
        return yellow_door::get_texture();
    case texture_type::YELLOW_KEY:
        return yellow_key::get_texture();
    }

    return clear::get_texture();
}

screen::Tile get_digit_texture(int digit) noexcept
{
    switch (digit)
    {
    case 0:
        return digit_00::get_texture();
    case 1:
        return digit_01::get_texture();
    case 2:
        return digit_02::get_texture();
    case 3:
        return digit_03::get_texture();
    case 4:
        return digit_04::get_texture();
    case 5:
        return digit_05::get_texture();
    case 6:
        return digit_06::get_texture();
    case 7:
        return digit_07::get_texture();
    case 8:
        return digit_08::get_texture();
    case 9:
        return digit_09::get_texture();
    }

    return digit_00::get_texture();
}

} // namespace chippie::texture