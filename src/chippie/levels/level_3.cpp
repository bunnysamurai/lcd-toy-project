#include "level_3.hpp"

#include "chippie/entities/centipede.hpp"
#include "chippie/entities/chippie_entity.hpp"
#include "chippie/entities/moveable_block.hpp"

#include <cstring>

namespace chippie::level_3
{

namespace
{

constexpr void draw_h_line(state &game_state, terrain_type terrain, uint32_t row, uint32_t col,
                           uint32_t length) noexcept
{
    for (uint32_t xx = col; xx < col + length; ++xx)
    {
        game_state.the_map[Grid::Location{.x = xx, .y = row}] = terrain;
        game_state.the_map[Grid::Location{.x = xx, .y = row}] = terrain;
    }
}

constexpr void draw_v_line(state &game_state, terrain_type terrain, uint32_t row, uint32_t col,
                           uint32_t length) noexcept
{
    for (uint32_t yy = row; yy < row + length; ++yy)
    {
        game_state.the_map[Grid::Location{.x = col, .y = yy}] = terrain;
        game_state.the_map[Grid::Location{.x = col, .y = yy}] = terrain;
    }
}

void load_level_from_rom_stub(state &game_state) noexcept
{
    /* set the time limit, if any */
    game_state.time_remaining = 100;
    game_state.countdown_timer.reset();

    /* set the amount of chips for the level */
    game_state.chippie_inventory.set(inventory_item::CHIPS, 4);

    /* set the hint string, which can be null */
    game_state.hint_text = "Suction-boots for force   floors. Fire boots for    fire. Flippers for water. Skates for ice.";
    /* set the level name */
    game_state.level_name_text = "LESSON 3\nPassword: ECBQ";

    /* clear the map data */
    std::memset(std::data(game_state.the_map.map_data), static_cast<int>(terrain_type::CLEAR),
                sizeof(terrain_type) * std::size(game_state.the_map.map_data));

    /* this is literally level 3... don't worry, I'll write a tool for this kind of thing... */

    /* outer wall */
    draw_h_line(game_state, terrain_type::WALL, 5, 15, 3);
    draw_h_line(game_state, terrain_type::WALL, 7, 11, 5);
    draw_h_line(game_state, terrain_type::WALL, 7, 17, 4);
    draw_h_line(game_state, terrain_type::WALL, 10, 8, 4);
    draw_h_line(game_state, terrain_type::WALL, 10, 20, 4);
    draw_h_line(game_state, terrain_type::WALL, 19, 8, 6);
    draw_h_line(game_state, terrain_type::WALL, 19, 18, 6);
    draw_h_line(game_state, terrain_type::WALL, 21, 13, 6);

    game_state.the_map[Grid::Location{.x = 15, .y = 6}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 17, .y = 6}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 11, .y = 8}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 11, .y = 9}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 20, .y = 8}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 20, .y = 9}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 13, .y = 20}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 18, .y = 20}] = terrain_type::WALL;

    draw_v_line(game_state, terrain_type::WALL, 11, 8, 8);
    draw_v_line(game_state, terrain_type::WALL, 11, 23, 8);

    /* inner wall */
    draw_h_line(game_state, terrain_type::WALL, 9, 13, 6);
    draw_h_line(game_state, terrain_type::WALL, 12, 10, 4);
    draw_h_line(game_state, terrain_type::WALL, 12, 18, 4);
    draw_h_line(game_state, terrain_type::WALL, 17, 10, 4);
    draw_h_line(game_state, terrain_type::WALL, 17, 18, 4);

    draw_v_line(game_state, terrain_type::WALL, 10, 13, 2);
    draw_v_line(game_state, terrain_type::WALL, 10, 18, 2);
    draw_v_line(game_state, terrain_type::WALL, 13, 10, 4);
    draw_v_line(game_state, terrain_type::WALL, 13, 21, 4);
    draw_v_line(game_state, terrain_type::WALL, 14, 13, 2);

    /* push floors */
    draw_v_line(game_state, terrain_type::PUSH_FLOOR_DOWN, 8, 12, 3);
    draw_h_line(game_state, terrain_type::PUSH_FLOOR_LEFT, 11, 10, 3);
    draw_v_line(game_state, terrain_type::PUSH_FLOOR_DOWN, 11, 9, 7);
    draw_h_line(game_state, terrain_type::PUSH_FLOOR_RIGHT, 18, 9, 5);
    draw_h_line(game_state, terrain_type::PUSH_FLOOR_UP, 18, 14, 3);
    game_state.the_map[Grid::Location{.x = 14, .y = 19}] = terrain_type::PUSH_FLOOR_UP;
    draw_h_line(game_state, terrain_type::PUSH_FLOOR_UP, 19, 16, 2);
    draw_h_line(game_state, terrain_type::PUSH_FLOOR_UP, 20, 14, 4);
    draw_h_line(game_state, terrain_type::PUSH_FLOOR_RIGHT, 18, 17, 5);
    draw_v_line(game_state, terrain_type::PUSH_FLOOR_UP, 12, 22, 7);
    draw_h_line(game_state, terrain_type::PUSH_FLOOR_LEFT, 11, 20, 3);
    draw_v_line(game_state, terrain_type::PUSH_FLOOR_UP, 9, 19, 3);
    draw_h_line(game_state, terrain_type::PUSH_FLOOR_LEFT, 8, 13, 7);
    game_state.the_map[Grid::Location{.x = 15, .y = 19}] = terrain_type::CHIP;

    /* water */
    draw_h_line(game_state, terrain_type::WATER, 10, 14, 4);
    draw_h_line(game_state, terrain_type::WATER, 12, 14, 4);
    game_state.the_map[Grid::Location{.x = 14, .y = 11}] = terrain_type::WATER;
    game_state.the_map[Grid::Location{.x = 17, .y = 11}] = terrain_type::WATER;
    game_state.the_map[Grid::Location{.x = 15, .y = 11}] = terrain_type::ICE_SKATES;
    game_state.the_map[Grid::Location{.x = 16, .y = 11}] = terrain_type::CHIP;

    /* ice */
    draw_h_line(game_state, terrain_type::ICE, 13, 12, 2);
    draw_v_line(game_state, terrain_type::ICE, 14, 11, 2);
    draw_h_line(game_state, terrain_type::ICE, 16, 12, 2);
    game_state.the_map[Grid::Location{.x = 11, .y = 13}] = terrain_type::ICE_TOPLEFT;
    game_state.the_map[Grid::Location{.x = 11, .y = 16}] = terrain_type::ICE_BOTLEFT;
    game_state.the_map[Grid::Location{.x = 12, .y = 14}] = terrain_type::FIRE_BOOTS;
    game_state.the_map[Grid::Location{.x = 12, .y = 15}] = terrain_type::CHIP;

    /* fire */
    draw_v_line(game_state, terrain_type::FIRE, 13, 18, 4);
    draw_v_line(game_state, terrain_type::FIRE, 13, 20, 4);
    game_state.the_map[Grid::Location{.x = 19, .y = 13}] = terrain_type::FIRE;
    game_state.the_map[Grid::Location{.x = 19, .y = 16}] = terrain_type::FIRE;
    game_state.the_map[Grid::Location{.x = 19, .y = 14}] = terrain_type::SUCTION_BOOTS;
    game_state.the_map[Grid::Location{.x = 19, .y = 15}] = terrain_type::CHIP;

    /* misc */
    game_state.the_map[Grid::Location{.x = 16, .y = 15}] = terrain_type::HINT;
    game_state.the_map[Grid::Location{.x = 16, .y = 16}] = terrain_type::FLIPPERS;
    game_state.the_map[Grid::Location{.x = 16, .y = 7}] = terrain_type::SOCKET;
    game_state.the_map[Grid::Location{.x = 16, .y = 6}] = terrain_type::PORTAL;

}

void load_entity_list_from_rom_stub(state &game_state) noexcept
{
    uint32_t uuid{0};

    game_state.entity_list.clear();

    // by convention, Chippie is always the first one in the entity_list
    game_state.entity_list.push_back(chippie::create(game_state, {.x = 15, .y = 15}, direction::DOWN, uuid++));
}

} // namespace

void load_level(state &game_state) noexcept
{
    load_level_from_rom_stub(game_state);
    load_entity_list_from_rom_stub(game_state);
}

} // namespace chippie::level_3