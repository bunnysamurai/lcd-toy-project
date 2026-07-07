#include "level_5.hpp"

#include "chippie/entities/blue_tank.hpp"
#include "chippie/entities/chippie_entity.hpp"
#include "chippie/entities/entity_types.hpp"
#include "chippie/entities/fire_dancer.hpp"
#include "chippie/entities/moveable_block.hpp"
#include "chippie/entities/purple_ball.hpp"
#include "chippie/entities/water_glider.hpp"

#include "level_utils.hpp"

#include <cstring>
#include <pico/time.h>

namespace chippie::level_5
{

namespace
{

void load_level_from_rom_stub(state &game_state) noexcept
{
    /* set the time limit, if any */
    game_state.time_remaining = 100;
    game_state.countdown_timer.reset();

    /* set the amount of chips for the level */
    game_state.chippie_inventory.set(inventory_item::CHIPS, 0);

    /* set the hint string, which can be null */
    game_state.hint_text = "The red button controls   the clone machine. The    brown button opens traps.";

    /* set the level name */
    game_state.level_name_text = "LESSON 5\nPassword: TQKB";

    /* clear the map data */
    std::memset(std::data(game_state.the_map.map_data), static_cast<int>(terrain_type::CLEAR),
                sizeof(terrain_type) * std::size(game_state.the_map.map_data));

    /* this is literally level 5... don't worry, I'll write a tool for this kind of thing... */

    /* walls */
    draw_h_line(game_state, terrain_type::WALL, 4, 10, 11);
    draw_h_line(game_state, terrain_type::WALL, 6, 10, 3);
    draw_h_line(game_state, terrain_type::WALL, 12, 12, 11);
    draw_h_line(game_state, terrain_type::WALL, 14, 14, 5);
    draw_h_line(game_state, terrain_type::WALL, 16, 12, 9);
    draw_h_line(game_state, terrain_type::WALL, 21, 12, 11);

    draw_v_line(game_state, terrain_type::WALL, 6, 12, 16);
    draw_v_line(game_state, terrain_type::WALL, 4, 20, 8);
    draw_v_line(game_state, terrain_type::WALL, 12, 22, 10);
    draw_v_line(game_state, terrain_type::WALL, 14, 20, 2);

    game_state.the_map[Grid::Location{.x = 10, .y = 5}] = terrain_type::WALL;

    /* bomb area */
    game_state.the_map[Grid::Location{.x = 12, .y = 5}] = terrain_type::BOMB;

    draw_h_line(game_state, terrain_type::BOMB, 6, 15, 2);
    draw_h_line(game_state, terrain_type::BOMB, 8, 15, 2);
    draw_h_line(game_state, terrain_type::BOMB, 9, 15, 2);
    draw_h_line(game_state, terrain_type::BOMB, 11, 15, 2);
    draw_v_line(game_state, terrain_type::BOMB, 6, 17, 6);

    game_state.the_map[Grid::Location{.x = 18, .y = 7}] = terrain_type::TRAP;
    game_state.the_map[Grid::Location{.x = 16, .y = 7}] = terrain_type::BROWN_BUTTON;
    game_state.the_map[Grid::Location{.x = 18, .y = 10}] = terrain_type::TRAP;
    game_state.the_map[Grid::Location{.x = 16, .y = 10}] = terrain_type::BROWN_BUTTON;

    /* start area */
    draw_v_line(game_state, terrain_type::FIRE, 17, 15, 3);
    draw_v_line(game_state, terrain_type::WATER, 17, 13, 3);

    game_state.the_map[Grid::Location{.x = 16, .y = 15}] = terrain_type::GREEN_BUTTON_WALL;
    game_state.the_map[Grid::Location{.x = 16, .y = 13}] = terrain_type::GREEN_BUTTON;
    game_state.the_map[Grid::Location{.x = 18, .y = 15}] = terrain_type::RED_BUTTON;
    game_state.the_map[Grid::Location{.x = 14, .y = 17}] = terrain_type::RED_KEY;
    game_state.the_map[Grid::Location{.x = 18, .y = 18}] = terrain_type::CLONER_FIRE_DANCER;
    game_state.the_map[Grid::Location{.x = 20, .y = 18}] = terrain_type::HINT;
    game_state.the_map[Grid::Location{.x = 14, .y = 12}] = terrain_type::RED_DOOR;

    /* misc */
    game_state.the_map[Grid::Location{.x = 11, .y = 5}] = terrain_type::PORTAL;
}

void load_entity_list_from_rom_stub(state &game_state) noexcept
{
    uint32_t uuid{0};

    game_state.entity_list.clear();

    // by convention, Chippie is always the first one in the entity_list
    game_state.entity_list.push_back(chippie::create(game_state, {.x = 20, .y = 19}, direction::DOWN, uuid++));
    game_state.entity_list.push_back(purple_ball::create(game_state, {.x = 17, .y = 15}, direction::RIGHT, uuid++));
    game_state.entity_list.push_back(water_glider::create(game_state, {.x = 18, .y = 11}, direction::UP, uuid++));

    /* add in the red and brown buttons to the list */
    game_state.red_button_list.push_back(red_button{
        .button = {.x = 18, .y = 15},
        .clone_spawn = {.x = 17, .y = 18},
        .clone_facing = direction::LEFT,
        .clone_type = entity_type::FIRE_DANCER,
    });

    game_state.brown_button_list.push_back(brown_button{
        .button = {.x = 16, .y = 10},
        .trap = {.x = 18, .y = 10},
    });
    game_state.brown_button_list.push_back(brown_button{
        .button = {.x = 16, .y = 7},
        .trap = {.x = 18, .y = 7},
    });
}

} // namespace

void load_level(state &game_state) noexcept
{
    load_level_from_rom_stub(game_state);
    load_entity_list_from_rom_stub(game_state);
}

} // namespace chippie::level_5
