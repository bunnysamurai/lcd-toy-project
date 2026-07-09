#include "level_8.hpp"

#include "chippie/entities/chippie_entity.hpp"
#include "chippie/entities/entity_types.hpp"
#include "chippie/entities/frog_monster.hpp"

#include "chippie/state/teleporter.hpp"
#include "level_utils.hpp"

#include <cstring>
#include <pico/time.h>

namespace chippie::level_8
{

namespace
{

void load_level_from_rom_stub(state &game_state) noexcept
{
    /* set the time limit, if any */
    game_state.time_remaining = 100;
    game_state.countdown_timer.reset();

    /* set the amount of chips for the level */
    game_state.chippie_inventory.set(inventory_item::CHIPS, 1);

    /* set the hint string, which can be null */
    game_state.hint_text = "Monsters are stopped by   dirt and gravel.";

    /* set the level name */
    game_state.level_name_text = "LESSON 8\nPassword: NHAG";

    /* clear the map data */
    std::memset(std::data(game_state.the_map.map_data), static_cast<int>(terrain_type::CLEAR),
                sizeof(terrain_type) * std::size(game_state.the_map.map_data));

    /* this is literally level 6... don't worry, I'll write a tool for this kind of thing... */

    /* walls */
    draw_h_line(game_state, terrain_type::WALL, 5, 0, 21);
    draw_h_line(game_state, terrain_type::WALL, 23, 0, 21);
    draw_h_line(game_state, terrain_type::WALL, 6, 0, 21);
    draw_h_line(game_state, terrain_type::WALL, 22, 0, 21);

    draw_v_line(game_state, terrain_type::WALL, 5, 0, 19);
    draw_v_line(game_state, terrain_type::WALL, 5, 20, 19);
    draw_v_line(game_state, terrain_type::WALL, 5, 1, 19);
    draw_v_line(game_state, terrain_type::WALL, 5, 19, 19);

    /* gravel */
    draw_h_line(game_state, terrain_type::GRAVEL, 7, 2, 17);
    draw_h_line(game_state, terrain_type::GRAVEL, 21, 2, 17);
    draw_v_line(game_state, terrain_type::GRAVEL, 7, 2, 15);
    draw_v_line(game_state, terrain_type::GRAVEL, 7, 3, 15);
    draw_v_line(game_state, terrain_type::GRAVEL, 7, 17, 15);
    draw_v_line(game_state, terrain_type::GRAVEL, 7, 18, 15);

    /* dirt */
    draw_h_line(game_state, terrain_type::DIRT, 8, 4, 13);
    draw_h_line(game_state, terrain_type::DIRT, 9, 4, 13);
    draw_h_line(game_state, terrain_type::DIRT, 19, 4, 13);
    draw_h_line(game_state, terrain_type::DIRT, 20, 4, 13);
    draw_v_line(game_state, terrain_type::DIRT, 8, 4, 13);
    draw_v_line(game_state, terrain_type::DIRT, 8, 5, 13);
    draw_v_line(game_state, terrain_type::DIRT, 8, 15, 13);
    draw_v_line(game_state, terrain_type::DIRT, 8, 16, 13);

    /* misc */
    game_state.the_map[Grid::Location{.x = 10, .y = 14}] = terrain_type::CHIP;
    game_state.the_map[Grid::Location{.x = 1, .y = 14}] = terrain_type::CLEAR;
    game_state.the_map[Grid::Location{.x = 2, .y = 13}] = terrain_type::HINT;
    game_state.the_map[Grid::Location{.x = 19, .y = 14}] = terrain_type::SOCKET;
    game_state.the_map[Grid::Location{.x = 20, .y = 14}] = terrain_type::PORTAL;
}

void load_entity_list_from_rom_stub(state &game_state) noexcept
{
    uint32_t uuid{0};

    game_state.entity_list.clear();

    // by convention, Chippie is always the first one in the entity_list
    game_state.entity_list.push_back(chippie::create(game_state, {.x = 1, .y = 14}, direction::DOWN, uuid++));
    game_state.entity_list.push_back(frog_monster::create(game_state, {.x = 9, .y = 14}, direction::UP, uuid++));
}

} // namespace

void load_level(state &game_state) noexcept
{
    load_level_from_rom_stub(game_state);
    load_entity_list_from_rom_stub(game_state);
}

} // namespace chippie::level_8
