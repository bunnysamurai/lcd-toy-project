#if !defined(TEXTURE_HPP)
#define TEXTURE_HPP

#include "chippie/entities/basic_entity.hpp"
#include "chippie/state/terrain_types.hpp"
#include "screen/TileDef.h"

namespace chippie::texture
{

enum struct texture_type
{
    CHIP,
    CHIPPIE_UP, /* maintain this ordering for entites with facings, please */
    CHIPPIE_RIGHT,
    CHIPPIE_DOWN,
    CHIPPIE_LEFT,
    CLEAR,
    CYAN_DOOR,
    CYAN_KEY,
    GREEN_DOOR,
    GREEN_KEY,
    HINT,
    PORTAL,
    RED_DOOR,
    RED_KEY,
    SOCKET,
    WALL,
    YELLOW_DOOR,
    YELLOW_KEY,
    CENTIPEDE_UP,
    CENTIPEDE_RIGHT,
    CENTIPEDE_DOWN,
    CENTIPEDE_LEFT,
    DIRT,
    MOVING_BLOCK,
    WATER_NOSPLASH,
    WATER_SPLASH,
    CAMPFIRE,
    PURPLE_BALL,
    DIGIT_00_YELLOW,
    FIRE_BOOTS,
    FLIPPERS,
    ICE_SKATES,
    SUCTION_BOOTS,
    ICE,
    ICE_TOPLEFT,
    ICE_TOPRIGHT,
    ICE_BOTTOMRIGHT,
    ICE_BOTTOMLEFT,
    PUSH_FLOOR_UP,
    PUSH_FLOOR_RIGHT,
    PUSH_FLOOR_DOWN,
    PUSH_FLOOR_LEFT,
    BLUE_TANK_UP,
    BLUE_TANK_RIGHT,
    BLUE_TANK_DOWN,
    BLUE_TANK_LEFT,
    GREEN_BUTTON_CLEAR,
    GREEN_BUTTON_WALL,
    GREEN_BUTTON,
    BLUE_BUTTON,
    BROWN_BUTTON,
    RED_BUTTON,
    BOMB,
    TRAP,
    WATER_GLIDER_UP,
    WATER_GLIDER_RIGHT,
    WATER_GLIDER_DOWN,
    WATER_GLIDER_LEFT,
    CLONER_FIRE_DANCER,
    FIRE_DANCER,
    MAGIC_WALL,
    THIEF,
    TELEPORTER,
    THIN_WALL_BOTTOM,
    WALL_TRAP,
    GRAVEL,
    FROG_UP,
    FROG_RIGHT,
    FROG_DOWN,
    FROG_LEFT,
    DIGIT_CLEAR,
    CYAN_STICK_BALL,
    CYAN_STICK_BALL_MIRRORED,
    THIN_WALL_TOP,
    THIN_WALL_RIGHT,
    THIN_WALL_LEFT,
    THIN_WALL_BOTRIGHT,
    BACTERIA_UP, /* maintain this ordering for entites with facings, please */
    BACTERIA_RIGHT,
    BACTERIA_DOWN,
    BACTERIA_LEFT,
    BLOB,
    CLONER_MOVEABLE_BLOCK,
    CLONER_FROG_MONSTER,
    PUSH_FLOOR_MULTI,
};

[[nodiscard]] texture_type get_terrain_texture_type(::chippie::terrain_type) noexcept;
[[nodiscard]] texture_type get_entity_texture_type(const ::chippie::entity &) noexcept;
[[nodiscard]] screen::Tile get_texture(texture_type) noexcept;
[[nodiscard]] screen::Tile get_digit_texture(int digit) noexcept;

} // namespace chippie::texture
#endif