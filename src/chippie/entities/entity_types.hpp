#include "revenge/revenge_tiles.hpp"
#if !defined(ENTITY_TYPES_HPP)
#define ENTITY_TYPES_HPP

#include <array>

namespace chippie
{

/* entities are only things that can move by themselves */
enum struct entity_type
{
    CHIPPIE,
    BLUE_TANK,
    WATER_GLIDER,
    FIRE_DANCER,
    PURPLE_BALL,
    CYAN_STICK_BALL,
    BACTERIA,
    FROG_MONSTER,
    CENTIPEDE,
    ENTITY_TYPE_SIZE
};

} // namespace chippie

#endif