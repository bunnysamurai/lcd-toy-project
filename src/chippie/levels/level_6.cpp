#include "level_6.hpp"

#include "chippie/entities/chippie_entity.hpp"
#include "chippie/entities/entity_types.hpp"

#include "level_utils.hpp"

#include <cstring>
#include <pico/time.h>

namespace chippie::level_6
{

namespace
{

void load_level_from_rom_stub(state &game_state) noexcept
{
    /* set the time limit, if any */
    game_state.time_remaining = 100;
    game_state.countdown_timer.reset();

    /* set the amount of chips for the level */
    game_state.chippie_inventory.set(inventory_item::CHIPS, 4);

    /* set the hint string, which can be null */
    game_state.hint_text = "Blue walls may be fake.   Some invisible walls neverappear. There can be extrachips.";

    /* set the level name */
    game_state.level_name_text = "LESSON 6\nPassword: WNLD";

    /* clear the map data */
    std::memset(std::data(game_state.the_map.map_data), static_cast<int>(terrain_type::CLEAR),
                sizeof(terrain_type) * std::size(game_state.the_map.map_data));

    /* this is literally level 6... don't worry, I'll write a tool for this kind of thing... */

    /* walls */
    draw_h_line(game_state, terrain_type::MAGIC_TILE_WALL, 8, 8, 17);
    draw_h_line(game_state, terrain_type::MAGIC_TILE_WALL, 11, 11, 2);
    draw_h_line(game_state, terrain_type::MAGIC_TILE_WALL, 11, 20, 4);
    game_state.the_map[Grid::Location{.x = 9, .y = 11}] = terrain_type::MAGIC_TILE_WALL;
    draw_h_line(game_state, terrain_type::MAGIC_TILE_WALL, 16, 8, 6);
    draw_h_line(game_state, terrain_type::MAGIC_TILE_WALL, 16, 19, 6);

    draw_v_line(game_state, terrain_type::MAGIC_TILE_WALL, 9, 8, 7);
    draw_v_line(game_state, terrain_type::MAGIC_TILE_WALL, 9, 24, 7);
    draw_v_line(game_state, terrain_type::MAGIC_TILE_WALL, 12, 14, 2);
    draw_v_line(game_state, terrain_type::MAGIC_TILE_WALL, 14, 18, 2);
    game_state.the_map[Grid::Location{.x = 14, .y = 15}] = terrain_type::MAGIC_TILE_WALL;
    game_state.the_map[Grid::Location{.x = 18, .y = 12}] = terrain_type::MAGIC_TILE_WALL;

    draw_h_line(game_state, terrain_type::WALL, 11, 14, 5);
    draw_h_line(game_state, terrain_type::WALL, 16, 14, 2);
    draw_h_line(game_state, terrain_type::WALL, 16, 17, 2);
    draw_h_line(game_state, terrain_type::WALL, 18, 15, 3);
    game_state.the_map[Grid::Location{.x = 15, .y = 17}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 17, .y = 17}] = terrain_type::WALL;

    /* sneaky walls */
    draw_v_line(game_state, terrain_type::INVISIBLE_WALL, 12, 11, 4);
    draw_v_line(game_state, terrain_type::APPEARING_WALL, 12, 21, 4);

    game_state.the_map[Grid::Location{.x = 10, .y = 11}] = terrain_type::MAGIC_TILE_CLEAR;
    game_state.the_map[Grid::Location{.x = 13, .y = 11}] = terrain_type::MAGIC_TILE_CLEAR;
    game_state.the_map[Grid::Location{.x = 19, .y = 11}] = terrain_type::MAGIC_TILE_CLEAR;
    game_state.the_map[Grid::Location{.x = 14, .y = 14}] = terrain_type::MAGIC_TILE_CLEAR;
    game_state.the_map[Grid::Location{.x = 18, .y = 13}] = terrain_type::MAGIC_TILE_CLEAR;

    /* misc */
    draw_v_line(game_state, terrain_type::CHIP, 12, 9, 4);
    draw_v_line(game_state, terrain_type::CHIP, 12, 23, 4);
    game_state.the_map[Grid::Location{.x = 16, .y = 13}] = terrain_type::HINT;
    game_state.the_map[Grid::Location{.x = 16, .y = 16}] = terrain_type::SOCKET;
    game_state.the_map[Grid::Location{.x = 16, .y = 17}] = terrain_type::PORTAL;
}

void load_entity_list_from_rom_stub(state &game_state) noexcept
{
    uint32_t uuid{0};

    game_state.entity_list.clear();

    // by convention, Chippie is always the first one in the entity_list
    game_state.entity_list.push_back(chippie::create(game_state, {.x = 16, .y = 14}, direction::DOWN, uuid++));
}

} // namespace

void load_level(state &game_state) noexcept
{
    load_level_from_rom_stub(game_state);
    load_entity_list_from_rom_stub(game_state);
}

} // namespace chippie::level_6
