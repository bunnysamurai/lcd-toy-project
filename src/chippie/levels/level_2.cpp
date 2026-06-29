#include "level_2.hpp"

#include "chippie/entities/centipede.hpp"
#include "chippie/entities/chippie_entity.hpp"
#include "chippie/entities/moveable_block.hpp"

#include <cstring>

namespace chippie::level_2
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
    game_state.hint_text = "Push blocks into water to make dirt. Watch out for  monsters.";
    /* set the level name */
    game_state.level_name_text = "LESSON 2\nPassword: JXMJ";

    /* clear the map data */
    std::memset(std::data(game_state.the_map.map_data), static_cast<int>(terrain_type::CLEAR),
                sizeof(terrain_type) * std::size(game_state.the_map.map_data));

    /* this is literally level 2... don't worry, I'll write a tool for this kind of thing... */

    for (uint32_t xx = 9; xx < 16; ++xx)
    {
        game_state.the_map[Grid::Location{.x = xx, .y = 7}] = terrain_type::WALL;
        game_state.the_map[Grid::Location{.x = xx, .y = 17}] = terrain_type::WALL;
    }

    for (uint32_t xx = 15; xx < 25; ++xx)
    {
        game_state.the_map[Grid::Location{.x = xx, .y = 10}] = terrain_type::WALL;
        game_state.the_map[Grid::Location{.x = xx, .y = 14}] = terrain_type::WALL;
    }

    for (uint32_t xx = 7; xx < 10; ++xx)
    {
        game_state.the_map[Grid::Location{.x = xx, .y = 11}] = terrain_type::WALL;
        game_state.the_map[Grid::Location{.x = xx, .y = 13}] = terrain_type::WALL;
    }

    for (uint32_t yy = 8; yy < 11; ++yy)
    {
        game_state.the_map[Grid::Location{.x = 9, .y = yy}] = terrain_type::WALL;
    }

    for (uint32_t yy = 14; yy < 17; ++yy)
    {
        game_state.the_map[Grid::Location{.x = 9, .y = yy}] = terrain_type::WALL;
    }

    for (uint32_t yy = 8; yy < 10; ++yy)
    {
        game_state.the_map[Grid::Location{.x = 15, .y = yy}] = terrain_type::WALL;
    }

    for (uint32_t yy = 15; yy < 17; ++yy)
    {
        game_state.the_map[Grid::Location{.x = 15, .y = yy}] = terrain_type::WALL;
    }

    for (uint32_t yy = 11; yy < 14; ++yy)
    {
        game_state.the_map[Grid::Location{.x = 24, .y = yy}] = terrain_type::WALL;
    }

    for (uint32_t yy = 10; yy < 15; ++yy)
    {
        game_state.the_map[Grid::Location{.x = 12, .y = yy}] = terrain_type::WALL;
    }

    game_state.the_map[Grid::Location{.x = 7, .y = 12}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 13, .y = 12}] = terrain_type::WALL;

    game_state.the_map[Grid::Location{.x = 22, .y = 12}] = terrain_type::HINT;

    game_state.the_map[Grid::Location{.x = 12, .y = 8}] = terrain_type::CHIP;
    game_state.the_map[Grid::Location{.x = 12, .y = 16}] = terrain_type::CHIP;
    game_state.the_map[Grid::Location{.x = 23, .y = 11}] = terrain_type::CHIP;
    game_state.the_map[Grid::Location{.x = 23, .y = 13}] = terrain_type::CHIP;

    for (uint32_t yy = 11; yy < 14; ++yy)
    {
        game_state.the_map[Grid::Location{.x = 16, .y = yy}] = terrain_type::WATER;
        game_state.the_map[Grid::Location{.x = 17, .y = yy}] = terrain_type::WATER;
    }

    game_state.the_map[Grid::Location{.x = 9, .y = 12}] = terrain_type::SOCKET;
    game_state.the_map[Grid::Location{.x = 8, .y = 12}] = terrain_type::PORTAL;
}

void load_entity_list_from_rom_stub(state &game_state) noexcept
{
    uint32_t uuid{0};

    game_state.entity_list.clear();

    // by convention, Chippie is always the first one in the entity_list
    game_state.entity_list.push_back(chippie::create(game_state, {.x = 21, .y = 12}, direction::DOWN, uuid++));

    game_state.entity_list.push_back(
        moveable_block_entity::create(game_state, {.x = 19, .y = 12}, direction::UP, uuid++));
    game_state.entity_list.push_back(
        moveable_block_entity::create(game_state, {.x = 20, .y = 12}, direction::UP, uuid++));

    game_state.entity_list.push_back(centipede::create(game_state, {.x = 13, .y = 11}, direction::UP, uuid++));
    game_state.entity_list.push_back(centipede::create(game_state, {.x = 14, .y = 12}, direction::UP, uuid++));
    game_state.entity_list.push_back(centipede::create(game_state, {.x = 13, .y = 13}, direction::UP, uuid++));
}

} // namespace

void load_level(state &game_state) noexcept
{
    load_level_from_rom_stub(game_state);
    load_entity_list_from_rom_stub(game_state);
}

} // namespace chippie::level_2