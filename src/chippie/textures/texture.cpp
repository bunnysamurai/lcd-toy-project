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

#include "tiles/centipede_down.hpp"
#include "tiles/centipede_left.hpp"
#include "tiles/centipede_right.hpp"
#include "tiles/centipede_up.hpp"
#include "tiles/dirt.hpp"
#include "tiles/moving_block.hpp"
#include "tiles/water_nosplash.hpp"
#include "tiles/water_splash.hpp"

#include "tiles/campfire.hpp"
#include "tiles/digit_00_yellow.hpp"
#include "tiles/fire_boots.hpp"
#include "tiles/flippers.hpp"
#include "tiles/ice.hpp"
#include "tiles/ice_bottomleft.hpp"
#include "tiles/ice_bottomright.hpp"
#include "tiles/ice_skates.hpp"
#include "tiles/ice_topleft.hpp"
#include "tiles/ice_topright.hpp"
#include "tiles/purple_ball.hpp"
#include "tiles/push_floor_down.hpp"
#include "tiles/push_floor_left.hpp"
#include "tiles/push_floor_right.hpp"
#include "tiles/push_floor_up.hpp"
#include "tiles/suction_boots.hpp"

#include "tiles/blue_button.hpp"
#include "tiles/brown_button.hpp"
#include "tiles/green_button_clear.hpp"
#include "tiles/green_button_wall.hpp"
#include "tiles/green_button.hpp"
#include "tiles/red_button.hpp"

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
    case terrain_type::DIRT:
        return texture_type::DIRT;
    case terrain_type::WATER:
        return texture_type::WATER_NOSPLASH;
    case terrain_type::FIRE:
        return texture_type::CAMPFIRE;
    case terrain_type::ICE:
        return texture_type::ICE;
    case terrain_type::ICE_TOPLEFT:
        return texture_type::ICE_TOPLEFT;
    case terrain_type::ICE_TOPRIGHT:
        return texture_type::ICE_TOPRIGHT;
    case terrain_type::ICE_BOTLEFT:
        return texture_type::ICE_BOTTOMLEFT;
    case terrain_type::ICE_BOTRIGHT:
        return texture_type::ICE_BOTTOMRIGHT;
    case terrain_type::PUSH_FLOOR_UP:
        return texture_type::PUSH_FLOOR_UP;
    case terrain_type::PUSH_FLOOR_DOWN:
        return texture_type::PUSH_FLOOR_DOWN;
    case terrain_type::PUSH_FLOOR_LEFT:
        return texture_type::PUSH_FLOOR_LEFT;
    case terrain_type::PUSH_FLOOR_RIGHT:
        return texture_type::PUSH_FLOOR_RIGHT;
    case terrain_type::FIRE_BOOTS:
        return texture_type::FIRE_BOOTS;
    case terrain_type::FLIPPERS:
        return texture_type::FLIPPERS;
    case terrain_type::ICE_SKATES:
        return texture_type::ICE_SKATES;
    case terrain_type::SUCTION_BOOTS:
        return texture_type::SUCTION_BOOTS;
    /* not yet implemented */
    case terrain_type::GRAVEL:
    case terrain_type::TRAP:
    case terrain_type::BOMB:
    case terrain_type::GREEN_BUTTON:
    case terrain_type::BLUE_BUTTON:
    case terrain_type::BROWN_BUTTON:
    case terrain_type::RED_BUTTON:
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
    case entity_type::BLUE_TANK_THAT_MOVES_DOWN:
        return texture_type::BLUE_TANK_DOWN;
    case entity_type::BLUE_TANK_THAT_MOVES_LEFT:
        return texture_type::BLUE_TANK_LEFT;
    case entity_type::BLUE_TANK_THAT_MOVES_RIGHT:
        return texture_type::BLUE_TANK_RIGHT;
    case entity_type::BLUE_TANK_THAT_MOVES_UP:
        return texture_type::BLUE_TANK_UP;
    case entity_type::WATER_GLIDER:
    case entity_type::FIRE_DANCER:
    case entity_type::PURPLE_BALL:
    case entity_type::CYAN_STICK_BALL:
    case entity_type::BACTERIA:
    case entity_type::FROG_MONSTER:
    case entity_type::CENTIPEDE:
        return static_cast<texture_type>(static_cast<int>(texture_type::CENTIPEDE_UP) + static_cast<int>(ent.facing));
    case entity_type::MOVEABLE_BLOCK:
        return texture_type::MOVING_BLOCK;
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
    case texture_type::CENTIPEDE_UP:
        return centipede_up::get_texture();
    case texture_type::CENTIPEDE_DOWN:
        return centipede_down::get_texture();
    case texture_type::CENTIPEDE_LEFT:
        return centipede_left::get_texture();
    case texture_type::CENTIPEDE_RIGHT:
        return centipede_right::get_texture();
    case texture_type::DIRT:
        return dirt::get_texture();
    case texture_type::MOVING_BLOCK:
        return moving_block::get_texture();
    case texture_type::WATER_NOSPLASH:
        return water_nosplash::get_texture();
    case texture_type::WATER_SPLASH:
        return water_splash::get_texture();
    case texture_type::CAMPFIRE:
        return campfire::get_texture();
    case texture_type::PURPLE_BALL:
        return purple_ball::get_texture();
    case texture_type::DIGIT_00_YELLOW:
        return digit_00_yellow::get_texture();
    case texture_type::FIRE_BOOTS:
        return fire_boots::get_texture();
    case texture_type::FLIPPERS:
        return flippers::get_texture();
    case texture_type::ICE_SKATES:
        return ice_skates::get_texture();
    case texture_type::SUCTION_BOOTS:
        return suction_boots::get_texture();
    case texture_type::ICE:
        return ice::get_texture();
    case texture_type::ICE_TOPLEFT:
        return ice_topleft::get_texture();
    case texture_type::ICE_TOPRIGHT:
        return ice_topright::get_texture();
    case texture_type::ICE_BOTTOMRIGHT:
        return ice_bottomright::get_texture();
    case texture_type::ICE_BOTTOMLEFT:
        return ice_bottomleft::get_texture();
    case texture_type::PUSH_FLOOR_UP:
        return push_floor_up::get_texture();
    case texture_type::PUSH_FLOOR_RIGHT:
        return push_floor_right::get_texture();
    case texture_type::PUSH_FLOOR_DOWN:
        return push_floor_down::get_texture();
    case texture_type::PUSH_FLOOR_LEFT:
        return push_floor_left::get_texture();
    case texture_type::BLUE_TANK_UP:
        return blue_tank_up::get_texture();
    case texture_type::BLUE_TANK_RIGHT:
        return blue_tank_right::get_texture();
    case texture_type::BLUE_TANK_DOWN:
        return blue_tank_down::get_texture();
    case texture_type::BLUE_TANK_LEFT:
        return blue_tank_left::get_texture();
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