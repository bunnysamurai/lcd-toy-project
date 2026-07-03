#include "level_4.hpp"

#include "chippie/entities/blue_tank.hpp"
#include "chippie/entities/chippie_entity.hpp"
#include "chippie/entities/moveable_block.hpp"
#include "level_utils.hpp"

#include <cstring>

namespace chippie::level_4
{

namespace
{

void load_level_from_rom_stub(state &game_state) noexcept
{
    /* set the time limit, if any */
    game_state.time_remaining = 150;
    game_state.countdown_timer.reset();

    /* set the amount of chips for the level */
    game_state.chippie_inventory.set(inventory_item::CHIPS, 9);

    /* set the hint string, which can be null */
    game_state.hint_text = "The blue button controls  the tanks. The green      button toggles walls.     Sometimes "
                           "things are      hidden under blocks.";
    /* set the level name */
    game_state.level_name_text = "LESSON 4\nPassword: YMCJ";

    /* clear the map data */
    std::memset(std::data(game_state.the_map.map_data), static_cast<int>(terrain_type::CLEAR),
                sizeof(terrain_type) * std::size(game_state.the_map.map_data));

    /* this is literally level 4... don't worry, I'll write a tool for this kind of thing... */

    /* outer perimeter */
    draw_h_line(game_state, terrain_type::WALL, 4, 4, 9);
    draw_h_line(game_state, terrain_type::WALL, 5, 12, 14);
    draw_h_line(game_state, terrain_type::WALL, 12, 4, 11);
    draw_h_line(game_state, terrain_type::WALL, 18, 8, 7);
    draw_h_line(game_state, terrain_type::WALL, 14, 14, 6);
    draw_h_line(game_state, terrain_type::WALL, 17, 19, 7);

    draw_v_line(game_state, terrain_type::WALL, 5, 4, 7);
    draw_v_line(game_state, terrain_type::WALL, 6, 12, 5);
    draw_v_line(game_state, terrain_type::WALL, 13, 8, 5);
    draw_v_line(game_state, terrain_type::WALL, 15, 14, 3);
    draw_v_line(game_state, terrain_type::WALL, 15, 19, 2);
    draw_v_line(game_state, terrain_type::WALL, 6, 25, 11);

    /* tank area */
    game_state.the_map[Grid::Location{.x = 7, .y = 6}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 7, .y = 10}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 5, .y = 5}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 5, .y = 11}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 5, .y = 7}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 5, .y = 9}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 11, .y = 7}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 11, .y = 9}] = terrain_type::WALL;
    draw_h_line(game_state, terrain_type::WALL, 6, 9, 3);
    draw_h_line(game_state, terrain_type::WALL, 8, 5, 5);
    draw_h_line(game_state, terrain_type::WALL, 10, 9, 3);

    /* toggle wall area */
    draw_h_line(game_state, terrain_type::WALL, 11, 19, 6);
    draw_h_line(game_state, terrain_type::WALL, 8, 22, 3);
    draw_h_line(game_state, terrain_type::WALL, 14, 22, 3);
    draw_v_line(game_state, terrain_type::WALL, 6, 19, 3);
    draw_v_line(game_state, terrain_type::WALL, 10, 19, 3);
    draw_v_line(game_state, terrain_type::WALL, 7, 22, 3);
    draw_v_line(game_state, terrain_type::WALL, 13, 22, 3);

    /* staring area */
    draw_v_line(game_state, terrain_type::WALL, 6, 13, 2);
    draw_h_line(game_state, terrain_type::WALL, 7, 15, 2);
    draw_v_line(game_state, terrain_type::WALL, 8, 16, 3);
    draw_h_line(game_state, terrain_type::WALL, 8, 17, 3);
    draw_v_line(game_state, terrain_type::WALL, 12, 16, 2);

    /* green button stuff */
    game_state.the_map[Grid::Location{.x = 19, .y = 9}] = terrain_type::GREEN_BUTTON_WALL;
    game_state.the_map[Grid::Location{.x = 22, .y = 10}] = terrain_type::GREEN_BUTTON_WALL;
    game_state.the_map[Grid::Location{.x = 22, .y = 12}] = terrain_type::GREEN_BUTTON_WALL;

    game_state.the_map[Grid::Location{.x = 19, .y = 13}] = terrain_type::GREEN_BUTTON_CLEAR;
    game_state.the_map[Grid::Location{.x = 22, .y = 6}] = terrain_type::GREEN_BUTTON_CLEAR;
    game_state.the_map[Grid::Location{.x = 22, .y = 16}] = terrain_type::GREEN_BUTTON_CLEAR;

    game_state.the_map[Grid::Location{.x = 21, .y = 14}] = terrain_type::GREEN_BUTTON;
    game_state.the_map[Grid::Location{.x = 21, .y = 8}] = terrain_type::GREEN_BUTTON;
    game_state.the_map[Grid::Location{.x = 18, .y = 11}] = terrain_type::GREEN_BUTTON;

    /* and the rest */
    game_state.the_map[Grid::Location{.x = 5, .y = 6}] = terrain_type::BLUE_BUTTON;
    game_state.the_map[Grid::Location{.x = 5, .y = 10}] = terrain_type::BLUE_BUTTON;
    game_state.the_map[Grid::Location{.x = 11, .y = 8}] = terrain_type::BLUE_BUTTON;

    game_state.the_map[Grid::Location{.x = 10, .y = 14}] = terrain_type::CHIP;
    game_state.the_map[Grid::Location{.x = 10, .y = 16}] = terrain_type::CHIP;
    game_state.the_map[Grid::Location{.x = 12, .y = 14}] = terrain_type::CHIP;
    game_state.the_map[Grid::Location{.x = 12, .y = 16}] = terrain_type::CHIP;
    game_state.the_map[Grid::Location{.x = 11, .y = 5}] = terrain_type::CHIP;
    game_state.the_map[Grid::Location{.x = 24, .y = 7}] = terrain_type::CHIP;
    game_state.the_map[Grid::Location{.x = 24, .y = 10}] = terrain_type::CHIP;
    game_state.the_map[Grid::Location{.x = 24, .y = 12}] = terrain_type::CHIP;
    game_state.the_map[Grid::Location{.x = 24, .y = 15}] = terrain_type::CHIP;

    game_state.the_map[Grid::Location{.x = 11, .y = 15}] = terrain_type::FIRE;
    game_state.the_map[Grid::Location{.x = 14, .y = 10}] = terrain_type::HINT;
    game_state.the_map[Grid::Location{.x = 14, .y = 7}] = terrain_type::SOCKET;
    game_state.the_map[Grid::Location{.x = 17, .y = 7}] = terrain_type::PORTAL;
}

void load_entity_list_from_rom_stub(state &game_state) noexcept
{
    uint32_t uuid{0};

    game_state.entity_list.clear();

    // by convention, Chippie is always the first one in the entity_list
    game_state.entity_list.push_back(chippie::create(game_state, {.x = 14, .y = 9}, direction::DOWN, uuid++));
    game_state.entity_list.push_back(blue_tank::create(game_state, {.x = 8, .y = 5}, direction::UP, uuid++));
    game_state.entity_list.push_back(blue_tank::create(game_state, {.x = 8, .y = 9}, direction::UP, uuid++));

    game_state.entity_list.push_back(
        moveable_block_entity::create(game_state, {.x = 10, .y = 14}, direction::UP, uuid++));
    game_state.entity_list.push_back(
        moveable_block_entity::create(game_state, {.x = 10, .y = 16}, direction::UP, uuid++));
    game_state.entity_list.push_back(
        moveable_block_entity::create(game_state, {.x = 11, .y = 15}, direction::UP, uuid++));
    game_state.entity_list.push_back(
        moveable_block_entity::create(game_state, {.x = 12, .y = 14}, direction::UP, uuid++));
    game_state.entity_list.push_back(
        moveable_block_entity::create(game_state, {.x = 12, .y = 16}, direction::UP, uuid++));
}

} // namespace

void load_level(state &game_state) noexcept
{
    load_level_from_rom_stub(game_state);
    load_entity_list_from_rom_stub(game_state);
}

} // namespace chippie::level_4
