#include "level_7.hpp"

#include "chippie/entities/chippie_entity.hpp"
#include "chippie/entities/entity_types.hpp"

#include "chippie/state/teleporter.hpp"
#include "level_utils.hpp"

#include <cstring>
#include <pico/time.h>

namespace chippie::level_7
{

namespace
{

void load_level_from_rom_stub(state &game_state) noexcept
{
    /* set the time limit, if any */
    game_state.time_remaining = 150;
    game_state.countdown_timer.reset();

    /* set the amount of chips for the level */
    game_state.chippie_inventory.set(inventory_item::CHIPS, 3);

    /* set the hint string, which can be null */
    game_state.hint_text =
        "Teleports can be          directional. The thief    takes your tools. New     walls can appear under    you.";

    /* set the level name */
    game_state.level_name_text = "LESSON 7\nPassword: FXQO";

    /* clear the map data */
    std::memset(std::data(game_state.the_map.map_data), static_cast<int>(terrain_type::CLEAR),
                sizeof(terrain_type) * std::size(game_state.the_map.map_data));

    /* this is literally level 6... don't worry, I'll write a tool for this kind of thing... */

    /* walls */
    draw_h_line(game_state, terrain_type::WALL, 6, 12, 14);
    draw_h_line(game_state, terrain_type::WALL, 8, 14, 10);
    draw_h_line(game_state, terrain_type::WALL, 8, 9, 4);
    draw_h_line(game_state, terrain_type::WALL, 10, 11, 2);
    draw_h_line(game_state, terrain_type::WALL, 10, 14, 8);
    draw_h_line(game_state, terrain_type::WALL, 15, 11, 13);
    draw_h_line(game_state, terrain_type::WALL, 18, 18, 4);
    draw_h_line(game_state, terrain_type::WALL, 20, 9, 15);
    draw_h_line(game_state, terrain_type::WALL, 17, 23, 3);

    game_state.the_map[Grid::Location{.x = 12, .y = 7}] = terrain_type::WALL;
    draw_v_line(game_state, terrain_type::WALL, 7, 25, 10);
    draw_v_line(game_state, terrain_type::WALL, 9, 23, 6);
    draw_v_line(game_state, terrain_type::WALL, 18, 23, 2);
    draw_v_line(game_state, terrain_type::WALL, 16, 21, 2);
    draw_v_line(game_state, terrain_type::WALL, 12, 19, 3);
    draw_v_line(game_state, terrain_type::WALL, 16, 13, 3);
    draw_v_line(game_state, terrain_type::WALL, 9, 9, 11);
    draw_v_line(game_state, terrain_type::WALL, 10, 16, 11);
    draw_v_line(game_state, terrain_type::WALL, 11, 11, 4);

    /* force floor */
    draw_v_line(game_state, terrain_type::PUSH_FLOOR_UP, 10, 10, 10);
    game_state.the_map[Grid::Location{.x = 10, .y = 9}] = terrain_type::PUSH_FLOOR_RIGHT;
    game_state.the_map[Grid::Location{.x = 12, .y = 9}] = terrain_type::PUSH_FLOOR_RIGHT;
    draw_v_line(game_state, terrain_type::PUSH_FLOOR_DOWN, 7, 13, 3);
    draw_h_line(game_state, terrain_type::PUSH_FLOOR_LEFT, 7, 14, 11);
    game_state.the_map[Grid::Location{.x = 14, .y = 9}] = terrain_type::PUSH_FLOOR_LEFT;
    draw_h_line(game_state, terrain_type::PUSH_FLOOR_LEFT, 9, 16, 7);
    draw_v_line(game_state, terrain_type::PUSH_FLOOR_UP, 10, 22, 5);
    draw_v_line(game_state, terrain_type::PUSH_FLOOR_UP, 8, 24, 9);
    draw_h_line(game_state, terrain_type::PUSH_FLOOR_RIGHT, 16, 22, 2);
    draw_v_line(game_state, terrain_type::PUSH_FLOOR_UP, 17, 22, 3);

    /* obstacles */
    draw_v_line(game_state, terrain_type::WATER, 16, 11, 4);
    draw_v_line(game_state, terrain_type::FIRE, 11, 21, 4);

    /* misc */
    game_state.the_map[Grid::Location{.x = 19, .y = 16}] = terrain_type::THIN_WALL_BOT;
    game_state.the_map[Grid::Location{.x = 20, .y = 16}] = terrain_type::THIN_WALL_BOT;

    game_state.the_map[Grid::Location{.x = 11, .y = 9}] = terrain_type::CHIP;
    game_state.the_map[Grid::Location{.x = 15, .y = 9}] = terrain_type::CHIP;
    game_state.the_map[Grid::Location{.x = 20, .y = 19}] = terrain_type::CHIP;
    game_state.the_map[Grid::Location{.x = 13, .y = 10}] = terrain_type::THIEF;
    game_state.the_map[Grid::Location{.x = 17, .y = 18}] = terrain_type::THIEF;
    game_state.the_map[Grid::Location{.x = 17, .y = 12}] = terrain_type::FLIPPERS;
    game_state.the_map[Grid::Location{.x = 15, .y = 18}] = terrain_type::FIRE_BOOTS;
    game_state.the_map[Grid::Location{.x = 15, .y = 14}] = terrain_type::TELEPORTER;
    game_state.the_map[Grid::Location{.x = 17, .y = 14}] = terrain_type::TELEPORTER;
    game_state.the_map[Grid::Location{.x = 15, .y = 16}] = terrain_type::TELEPORTER;
    game_state.the_map[Grid::Location{.x = 17, .y = 16}] = terrain_type::TELEPORTER;
    game_state.the_map[Grid::Location{.x = 19, .y = 11}] = terrain_type::WALL_TRAP;
    game_state.the_map[Grid::Location{.x = 13, .y = 19}] = terrain_type::WALL_TRAP;
    game_state.the_map[Grid::Location{.x = 14, .y = 12}] = terrain_type::HINT;
    game_state.the_map[Grid::Location{.x = 19, .y = 17}] = terrain_type::SOCKET;
    game_state.the_map[Grid::Location{.x = 20, .y = 17}] = terrain_type::PORTAL;

    /* teleporters */
    game_state.teleport_list.push_back(teleporter{.entry_location = {.x = 15, .y = 14}});
    game_state.teleport_list.back().set_exit(direction::RIGHT, {.x = 17, .y = 16}, direction::RIGHT);
    game_state.teleport_list.back().set_exit(direction::DOWN, {.x = 17, .y = 16}, direction::DOWN);

    game_state.teleport_list.push_back(teleporter{.entry_location = {.x = 17, .y = 14}});
    game_state.teleport_list.back().set_exit(direction::LEFT, {.x = 15, .y = 14}, direction::LEFT);
    game_state.teleport_list.back().set_exit(direction::DOWN, {.x = 17, .y = 16}, direction::DOWN);

    game_state.teleport_list.push_back(teleporter{.entry_location = {.x = 15, .y = 16}});
    game_state.teleport_list.back().set_exit(direction::RIGHT, {.x = 17, .y = 14}, direction::RIGHT);
    game_state.teleport_list.back().set_exit(direction::UP, {.x = 17, .y = 14}, direction::UP);

    game_state.teleport_list.push_back(teleporter{.entry_location = {.x = 17, .y = 16}});
    game_state.teleport_list.back().set_exit(direction::LEFT, {.x = 15, .y = 16}, direction::LEFT);
    game_state.teleport_list.back().set_exit(direction::UP, {.x = 17, .y = 14}, direction::UP);

    static_assert(decltype(game_state.teleport_list){}.capacity() >= 4);
}

void load_entity_list_from_rom_stub(state &game_state) noexcept
{
    uint32_t uuid{0};

    game_state.entity_list.clear();

    // by convention, Chippie is always the first one in the entity_list
    game_state.entity_list.push_back(chippie::create(game_state, {.x = 13, .y = 12}, direction::DOWN, uuid++));
}

} // namespace

void load_level(state &game_state) noexcept
{
    load_level_from_rom_stub(game_state);
    load_entity_list_from_rom_stub(game_state);
}

} // namespace chippie::level_7
