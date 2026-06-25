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
    CHIPPIE_DOWN,
    CHIPPIE_LEFT,
    CHIPPIE_RIGHT,
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
    CENTIPEDE_DOWN,
    CENTIPEDE_LEFT,
    CENTIPEDE_RIGHT,
    DIRT,
    MOVING_BLOCK,
    WATER_NOSPLASH,
    WATER_SPLASH
};

[[nodiscard]] texture_type get_terrain_texture_type(::chippie::terrain_type) noexcept;
[[nodiscard]] texture_type get_entity_texture_type(const ::chippie::entity &) noexcept;
[[nodiscard]] screen::Tile get_texture(texture_type) noexcept;
[[nodiscard]] screen::Tile get_digit_texture(int digit) noexcept;

} // namespace chippie::texture
#endif