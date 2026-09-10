#include "basic_entity.hpp"

#include "bacteria.hpp"
#include "blob.hpp"
#include "blue_tank.hpp"
#include "centipede.hpp"
#include "chippie/entities/entity_types.hpp"
#include "chippie_entity.hpp"
#include "cyan_stick_ball.hpp"
#include "fire_dancer.hpp"
#include "frog_monster.hpp"
#include "moveable_block.hpp"
#include "purple_ball.hpp"
#include "water_glider.hpp"

#include "chippie/state/state.hpp"

#include <algorithm>

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
        return water_glider::get_state_functions();
    case entity_type::FIRE_DANCER:
        return fire_dancer::get_state_functions();
    case entity_type::PURPLE_BALL:
        return purple_ball::get_state_functions();
    case entity_type::CYAN_STICK_BALL:
        return cyan_stick_ball::get_state_functions();
    case entity_type::BACTERIA:
        return bacteria::get_state_functions();
    case entity_type::FROG_MONSTER:
        return frog_monster::get_state_functions();
    case entity_type::CENTIPEDE:
        return centipede::get_state_functions();
    case entity_type::MOVEABLE_BLOCK:
        return moveable_block_entity::get_state_functions();
    case entity_type::BLOB:
        return blob::get_state_functions();
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
        return water_glider::get_velocity();
    case entity_type::FIRE_DANCER:
        return fire_dancer::get_velocity();
    case entity_type::PURPLE_BALL:
        return purple_ball::get_velocity();
    case entity_type::CYAN_STICK_BALL:
        return cyan_stick_ball::get_velocity();
    case entity_type::BACTERIA:
        return bacteria::get_velocity();
    case entity_type::FROG_MONSTER:
        return frog_monster::get_velocity();
    case entity_type::CENTIPEDE:
        return centipede::get_velocity();
    case entity_type::MOVEABLE_BLOCK:
        return moveable_block_entity::get_velocity();
    case entity_type::BLOB:
        return blob::get_velocity();
    case entity_type::ENTITY_TYPE_SIZE:
        return {};
    }
    return {};
}

entity create_entity(entity_type ent_id, Grid::Location location, direction facing, uint8_t uuid, state &game_state,
                     uint64_t next_time) noexcept
{
    switch (ent_id)
    {
    case entity_type::CHIPPIE:
        return chippie::create(game_state, location, facing, uuid);
    case entity_type::BLUE_TANK_THAT_MOVES_DOWN:
    case entity_type::BLUE_TANK_THAT_MOVES_LEFT:
    case entity_type::BLUE_TANK_THAT_MOVES_RIGHT:
    case entity_type::BLUE_TANK_THAT_MOVES_UP:
        return blue_tank::create(game_state, location, facing, uuid, next_time);
    case entity_type::WATER_GLIDER:
        return water_glider::create(game_state, location, facing, uuid, next_time);
    case entity_type::FIRE_DANCER:
        return fire_dancer::create(game_state, location, facing, uuid, next_time);
    case entity_type::PURPLE_BALL:
        return purple_ball::create(game_state, location, facing, uuid, next_time);
    case entity_type::CYAN_STICK_BALL:
        return cyan_stick_ball::create(game_state, location, facing, uuid, next_time);
    case entity_type::BACTERIA:
        return bacteria::create(game_state, location, facing, uuid, next_time);
    case entity_type::FROG_MONSTER:
        return frog_monster::create(game_state, location, facing, uuid, next_time);
    case entity_type::CENTIPEDE:
        return centipede::create(game_state, location, facing, uuid, next_time);
    case entity_type::MOVEABLE_BLOCK:
        return moveable_block_entity::create(game_state, location, facing, uuid, next_time);
    case entity_type::BLOB:
        return blob::create(game_state, location, facing, uuid, next_time);
    case entity_type::ENTITY_TYPE_SIZE:
        return {};
    }
    return {};
}

void enforce_superposition_principle(entity &myself) noexcept
{
    /* only apply superposition principle if we weren't just created by a red button */
    if (myself.has_summoning_sickness)
    {
        return;
    }

    auto &game_state{*myself.game_state};

    /* search for other entities */
    auto entitr =
        std::find_if(std::begin(game_state.entity_list), std::end(game_state.entity_list),
                     [&](const auto &other) { return (other.loc == myself.loc) && (myself.uuid != other.uuid); });

    /* if we occupy a space with another entity, sorry, but we need to die. */
    if (entitr != std::end(game_state.entity_list))
    {
        myself.alive = false;
    }
}

} // namespace chippie
