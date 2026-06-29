#include "level_1.hpp"

#include "chippie/entities/chippie_entity.hpp"

#include <cstring>

namespace chippie::level_1
{

namespace
{

void load_level_from_rom_stub(state &game_state) noexcept
{
    /* set the time limit, if any */
    game_state.time_remaining = 100;
    game_state.countdown_timer.reset();

    /* set the amount of chips for the level */
    game_state.chippie_inventory.set(inventory_item::CHIPS, 11);

    /* set the hint string, which can be null */
    game_state.hint_text = "Collect chips to get past the chip socket. Use keys to open doors.";
    /* set the level name */
    game_state.level_name_text = "LESSON 1\nPassword: BDHP";

    /* clear the map data */
    std::memset(std::data(game_state.the_map.map_data), static_cast<int>(terrain_type::CLEAR),
                sizeof(terrain_type) * std::size(game_state.the_map.map_data));

    /* this is literally level 1... don't worry, I'll write a tool for this kind of thing */
    game_state.the_map[Grid::Location{.x = 10, .y = 8}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 11, .y = 8}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 12, .y = 8}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 13, .y = 8}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 14, .y = 8}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 16, .y = 8}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 17, .y = 8}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 18, .y = 8}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 19, .y = 8}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 20, .y = 8}] = terrain_type::WALL;

    game_state.the_map[Grid::Location{.x = 10, .y = 9}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 12, .y = 9}] = terrain_type::CHIP;
    game_state.the_map[Grid::Location{.x = 14, .y = 9}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 15, .y = 9}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 16, .y = 9}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 18, .y = 9}] = terrain_type::CHIP;
    game_state.the_map[Grid::Location{.x = 20, .y = 9}] = terrain_type::WALL;

    game_state.the_map[Grid::Location{.x = 10, .y = 10}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 14, .y = 10}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 15, .y = 10}] = terrain_type::PORTAL;
    game_state.the_map[Grid::Location{.x = 16, .y = 10}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 20, .y = 10}] = terrain_type::WALL;

    game_state.the_map[Grid::Location{.x = 8, .y = 11}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 9, .y = 11}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 10, .y = 11}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 11, .y = 11}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 12, .y = 11}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 13, .y = 11}] = terrain_type::GREEN_DOOR;
    game_state.the_map[Grid::Location{.x = 14, .y = 11}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 15, .y = 11}] = terrain_type::SOCKET;
    game_state.the_map[Grid::Location{.x = 16, .y = 11}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 17, .y = 11}] = terrain_type::GREEN_DOOR;
    game_state.the_map[Grid::Location{.x = 18, .y = 11}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 19, .y = 11}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 20, .y = 11}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 21, .y = 11}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 22, .y = 11}] = terrain_type::WALL;

    game_state.the_map[Grid::Location{.x = 8, .y = 12}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 10, .y = 12}] = terrain_type::YELLOW_KEY;
    game_state.the_map[Grid::Location{.x = 12, .y = 12}] = terrain_type::CYAN_DOOR;
    game_state.the_map[Grid::Location{.x = 18, .y = 12}] = terrain_type::RED_DOOR;
    game_state.the_map[Grid::Location{.x = 20, .y = 12}] = terrain_type::YELLOW_KEY;
    game_state.the_map[Grid::Location{.x = 22, .y = 12}] = terrain_type::WALL;

    game_state.the_map[Grid::Location{.x = 8, .y = 13}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 10, .y = 13}] = terrain_type::CHIP;
    game_state.the_map[Grid::Location{.x = 12, .y = 13}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 13, .y = 13}] = terrain_type::CYAN_KEY;
    game_state.the_map[Grid::Location{.x = 15, .y = 13}] = terrain_type::HINT;
    game_state.the_map[Grid::Location{.x = 17, .y = 13}] = terrain_type::RED_KEY;
    game_state.the_map[Grid::Location{.x = 18, .y = 13}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 20, .y = 13}] = terrain_type::CHIP;
    game_state.the_map[Grid::Location{.x = 22, .y = 13}] = terrain_type::WALL;

    game_state.the_map[Grid::Location{.x = 8, .y = 14}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 9, .y = 14}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 10, .y = 14}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 11, .y = 14}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 12, .y = 14}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 13, .y = 14}] = terrain_type::CHIP;
    game_state.the_map[Grid::Location{.x = 17, .y = 14}] = terrain_type::CHIP;
    game_state.the_map[Grid::Location{.x = 18, .y = 14}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 19, .y = 14}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 20, .y = 14}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 21, .y = 14}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 22, .y = 14}] = terrain_type::WALL;

    game_state.the_map[Grid::Location{.x = 8, .y = 15}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 12, .y = 15}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 18, .y = 15}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 22, .y = 15}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 10, .y = 15}] = terrain_type::CHIP;
    game_state.the_map[Grid::Location{.x = 20, .y = 15}] = terrain_type::CHIP;
    game_state.the_map[Grid::Location{.x = 13, .y = 15}] = terrain_type::CYAN_KEY;
    game_state.the_map[Grid::Location{.x = 17, .y = 15}] = terrain_type::RED_KEY;

    game_state.the_map[Grid::Location{.x = 8, .y = 16}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 12, .y = 16}] = terrain_type::RED_DOOR;
    game_state.the_map[Grid::Location{.x = 18, .y = 16}] = terrain_type::CYAN_DOOR;
    game_state.the_map[Grid::Location{.x = 22, .y = 16}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 15, .y = 16}] = terrain_type::CHIP;

    for (uint32_t xx = 8; xx < 23; ++xx)
    {
        if (xx != 14 && xx != 16)
        {
            game_state.the_map[Grid::Location{.x = xx, .y = 17}] = terrain_type::WALL;
        }
        else
        {
            game_state.the_map[Grid::Location{.x = xx, .y = 17}] = terrain_type::YELLOW_DOOR;
        }
    }

    /* row 18 */
    game_state.the_map[Grid::Location{.x = 12, .y = 18}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 15, .y = 18}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 18, .y = 18}] = terrain_type::WALL;

    /* row 19 */
    game_state.the_map[Grid::Location{.x = 12, .y = 19}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 15, .y = 19}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 18, .y = 19}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 14, .y = 19}] = terrain_type::CHIP;
    game_state.the_map[Grid::Location{.x = 16, .y = 19}] = terrain_type::CHIP;

    /* row 20 */
    game_state.the_map[Grid::Location{.x = 12, .y = 20}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 15, .y = 20}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 18, .y = 20}] = terrain_type::WALL;
    game_state.the_map[Grid::Location{.x = 16, .y = 20}] = terrain_type::GREEN_KEY;

    /* row 21 */
    for (uint32_t xx = 12; xx < 19; ++xx)
    {
        game_state.the_map[Grid::Location{.x = xx, .y = 21}] = terrain_type::WALL;
    }
}

void load_entity_list_from_rom_stub(state &game_state) noexcept
{
    game_state.entity_list.clear();
    // by convention, Chippie is always the first one in the entity_list
    game_state.entity_list.push_back(chippie::create(game_state, {.x = 15, .y = 14}, direction::DOWN, 0));
}

} // namespace

void load_level(state &game_state) noexcept
{
    load_level_from_rom_stub(game_state);
    load_entity_list_from_rom_stub(game_state);
}

} // namespace chippie::level_1