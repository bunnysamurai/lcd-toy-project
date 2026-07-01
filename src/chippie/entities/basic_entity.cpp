#include "basic_entity.hpp"

#include "blue_tank.hpp"
#include "centipede.hpp"
#include "chippie_entity.hpp"
#include "moveable_block.hpp"
#include "purple_ball.hpp"

namespace chippie
{

[[nodiscard]] entity_state_machine get_state_functions(entity_type id) noexcept
{
    switch (id)
    {
    case entity_type::CHIPPIE:
        return chippie::get_state_functions();
    case entity_type::BLUE_TANK_THAT_MOVES_DOWN:
    case entity_type::BLUE_TANK_THAT_MOVES_LEFT:
    case entity_type::BLUE_TANK_THAT_MOVES_RIGHT:
    case entity_type::BLUE_TANK_THAT_MOVES_UP:
        return blue_tank::get_state_functions();
    case entity_type::WATER_GLIDER:
        return {};
    case entity_type::FIRE_DANCER:
        return {};
    case entity_type::PURPLE_BALL:
        return purple_ball::get_state_functions();
    case entity_type::CYAN_STICK_BALL:
        return {};
    case entity_type::BACTERIA:
        return {};
    case entity_type::FROG_MONSTER:
        return {};
    case entity_type::CENTIPEDE:
        return centipede::get_state_functions();
    case entity_type::MOVEABLE_BLOCK:
        return moveable_block_entity::get_state_functions();
    case entity_type::ENTITY_TYPE_SIZE:
        return {};
    }
    return {};
}

[[nodiscard]] uint64_t get_entity_velocity(entity_type id) noexcept
{
    switch (id)
    {
    case entity_type::CHIPPIE:
        return chippie::get_velocity();
    case entity_type::BLUE_TANK_THAT_MOVES_DOWN:
    case entity_type::BLUE_TANK_THAT_MOVES_LEFT:
    case entity_type::BLUE_TANK_THAT_MOVES_RIGHT:
    case entity_type::BLUE_TANK_THAT_MOVES_UP:
        return blue_tank::get_velocity();
    case entity_type::WATER_GLIDER:
        return {};
    case entity_type::FIRE_DANCER:
        return {};
    case entity_type::PURPLE_BALL:
        return purple_ball::get_velocity();
    case entity_type::CYAN_STICK_BALL:
        return {};
    case entity_type::BACTERIA:
        return {};
    case entity_type::FROG_MONSTER:
        return {};
    case entity_type::CENTIPEDE:
        return centipede::get_velocity();
    case entity_type::MOVEABLE_BLOCK:
        return moveable_block_entity::get_velocity();
    case entity_type::ENTITY_TYPE_SIZE:
        return {};
    }
    return {};
}

} // namespace chippie