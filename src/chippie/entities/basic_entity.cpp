#include "basic_entity.hpp"

#include "chippie_entity.hpp"
#include "purple_ball.hpp"

namespace chippie
{

[[nodiscard]] entity_state_machine get_state_functions(entity_type id) noexcept
{
    switch (id)
    {
    case entity_type::CHIPPIE:
        return chippie::get_state_functions();
    case entity_type::BLUE_TANK:
        return {};
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
        return {};
    case entity_type::ENTITY_TYPE_SIZE:
        return {};
    }
}

[[nodiscard]] uint64_t get_entity_velocity(entity_type id) noexcept
{
    switch (id)
    {
    case entity_type::CHIPPIE:
        return chippie::get_velocity();
    case entity_type::BLUE_TANK:
        return {};
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
        return {};
    case entity_type::ENTITY_TYPE_SIZE:
        return {};
    }
}

} // namespace chippie