#include "state.hpp"

#include "chippie/chippie_common.hpp"
#include "chippie/entities/basic_entity.hpp"
#include "chippie/entities/chippie_entity.hpp"
#include "chippie/entities/entity_types.hpp"
#include "chippie/events/event.hpp"
#include "chippie/render/paint_utils.hpp"
#include "chippie/state/chippie_inventory.hpp"
#include "chippie/textures/texture.hpp"
#include "chippie/textures/texture_defs.hpp"
#include "common/Rect.hpp"
#include "gamepad/gamepad.hpp"
#include "screen/screen.hpp"
#include "static_map.hpp"
#include "terrain_effects.hpp"

#include "embp/containers.hpp"

#include "pico/time.h"

#include <algorithm>
#include <cstring>
#include <pico/types.h>

#define DEBUG_PRINT
#ifdef DEBUG_PRINT
#include <pico/printf.h>
#endif

namespace chippie
{

bool State::is_active() noexcept
{
    return active;
}

void State::load_level() noexcept
{
    init_play_grid();
    load_level_from_rom_stub(level_number);
    load_entity_list_from_rom_stub(level_number);
}

void State::process() noexcept
{
    move_entities();

    /* TODO event processing, which may change the size of entity_list as well as make changes to the_map */
    event::process_events();

    /* entity cleanup */
    auto first_dead_itr{
        std::partition(std::begin(entity_list), std::end(entity_list), [](const auto &ent) { return ent.alive; })};
    entity_list.resize(std::distance(std::begin(entity_list), first_dead_itr));

    /* paint the entire frame every 33 ms */
    if (absolute_time_diff_us(last_paint_time, get_absolute_time()) > PAINT_TIME_INTERVAL_US)
    {
        last_paint_time = delayed_by_us(last_paint_time, PAINT_TIME_INTERVAL_US);
        paint();
    }

    if (event::exit())
    {
        /* TODO do an end-of-game thing? */
        active = false;
    }
}

void State::paint() noexcept
{
    screen::pause_screen();

    paint_the_map();
    paint_chip_count_display();
    paint_timer_display();
    paint_level_display();

    for (const auto &ent : entity_list)
    {
        if (within_view_port(ent.loc))
        {
            paint_entity(ent);
        }
    }

    screen::resume_screen();
}

void State::decrement_chip_count() noexcept
{
    if (chip_count > 0)
    {
        --chip_count;
    }
}

[[nodiscard]] uint16_t State::get_chip_count() noexcept
{
    return chip_count;
}

void State::load_entity_list_from_rom_stub([[maybe_unused]] uint8_t level) noexcept
{
    entity_list.clear();
    // by convention, Chippie is always the first one in the entity_list
    entity_list.push_back(chippie::create({.x = 15, .y = 14}, direction::DOWN, 0));
}

void State::load_level_from_rom_stub(uint8_t level) noexcept
{
    /* set the amount of chips for the level */
    access_chippie_inventory().set(inventory_item::CHIPS, 11);

    /* clear the map data */
    std::memset(std::data(the_map.map_data), static_cast<int>(terrain_type::CLEAR),
                sizeof(terrain_type) * std::size(the_map.map_data));

    /* this is literally level 1... don't worry, I'll write a tool for this kind of thing */
    the_map[Grid::Location{.x = 10, .y = 8}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 11, .y = 8}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 12, .y = 8}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 13, .y = 8}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 14, .y = 8}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 16, .y = 8}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 17, .y = 8}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 18, .y = 8}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 19, .y = 8}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 20, .y = 8}] = terrain_type::WALL;

    the_map[Grid::Location{.x = 10, .y = 9}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 12, .y = 9}] = terrain_type::CHIP;
    the_map[Grid::Location{.x = 14, .y = 9}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 15, .y = 9}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 16, .y = 9}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 18, .y = 9}] = terrain_type::CHIP;
    the_map[Grid::Location{.x = 20, .y = 9}] = terrain_type::WALL;

    the_map[Grid::Location{.x = 10, .y = 10}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 14, .y = 10}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 15, .y = 10}] = terrain_type::PORTAL;
    the_map[Grid::Location{.x = 16, .y = 10}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 20, .y = 10}] = terrain_type::WALL;

    the_map[Grid::Location{.x = 8, .y = 11}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 9, .y = 11}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 10, .y = 11}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 11, .y = 11}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 12, .y = 11}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 13, .y = 11}] = terrain_type::GREEN_DOOR;
    the_map[Grid::Location{.x = 14, .y = 11}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 15, .y = 11}] = terrain_type::SOCKET;
    the_map[Grid::Location{.x = 16, .y = 11}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 17, .y = 11}] = terrain_type::GREEN_DOOR;
    the_map[Grid::Location{.x = 18, .y = 11}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 19, .y = 11}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 20, .y = 11}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 21, .y = 11}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 22, .y = 11}] = terrain_type::WALL;

    the_map[Grid::Location{.x = 8, .y = 12}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 10, .y = 12}] = terrain_type::YELLOW_KEY;
    the_map[Grid::Location{.x = 12, .y = 12}] = terrain_type::CYAN_DOOR;
    the_map[Grid::Location{.x = 18, .y = 12}] = terrain_type::RED_DOOR;
    the_map[Grid::Location{.x = 20, .y = 12}] = terrain_type::YELLOW_KEY;
    the_map[Grid::Location{.x = 22, .y = 12}] = terrain_type::WALL;

    the_map[Grid::Location{.x = 8, .y = 13}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 10, .y = 13}] = terrain_type::CHIP;
    the_map[Grid::Location{.x = 12, .y = 13}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 13, .y = 13}] = terrain_type::CYAN_KEY;
    the_map[Grid::Location{.x = 15, .y = 13}] = terrain_type::HINT;
    the_map[Grid::Location{.x = 17, .y = 13}] = terrain_type::RED_KEY;
    the_map[Grid::Location{.x = 18, .y = 13}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 20, .y = 13}] = terrain_type::CHIP;
    the_map[Grid::Location{.x = 22, .y = 13}] = terrain_type::WALL;

    the_map[Grid::Location{.x = 8, .y = 14}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 9, .y = 14}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 10, .y = 14}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 11, .y = 14}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 12, .y = 14}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 13, .y = 14}] = terrain_type::CHIP;
    the_map[Grid::Location{.x = 17, .y = 14}] = terrain_type::CHIP;
    the_map[Grid::Location{.x = 18, .y = 14}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 19, .y = 14}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 20, .y = 14}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 21, .y = 14}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 22, .y = 14}] = terrain_type::WALL;

    the_map[Grid::Location{.x = 8, .y = 15}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 12, .y = 15}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 18, .y = 15}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 22, .y = 15}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 10, .y = 15}] = terrain_type::CHIP;
    the_map[Grid::Location{.x = 20, .y = 15}] = terrain_type::CHIP;
    the_map[Grid::Location{.x = 13, .y = 15}] = terrain_type::CYAN_KEY;
    the_map[Grid::Location{.x = 17, .y = 15}] = terrain_type::RED_KEY;

    the_map[Grid::Location{.x = 8, .y = 16}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 12, .y = 16}] = terrain_type::RED_DOOR;
    the_map[Grid::Location{.x = 18, .y = 16}] = terrain_type::CYAN_DOOR;
    the_map[Grid::Location{.x = 22, .y = 16}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 15, .y = 16}] = terrain_type::CHIP;

    for (uint32_t xx = 8; xx < 23; ++xx)
    {
        if (xx != 14 && xx != 16)
        {
            the_map[Grid::Location{.x = xx, .y = 17}] = terrain_type::WALL;
        }
        else
        {
            the_map[Grid::Location{.x = xx, .y = 17}] = terrain_type::YELLOW_DOOR;
        }
    }

    /* row 18 */
    the_map[Grid::Location{.x = 12, .y = 18}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 15, .y = 18}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 18, .y = 18}] = terrain_type::WALL;

    /* row 19 */
    the_map[Grid::Location{.x = 12, .y = 19}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 15, .y = 19}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 18, .y = 19}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 14, .y = 19}] = terrain_type::CHIP;
    the_map[Grid::Location{.x = 16, .y = 19}] = terrain_type::CHIP;

    /* row 20 */
    the_map[Grid::Location{.x = 12, .y = 20}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 15, .y = 20}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 18, .y = 20}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 16, .y = 20}] = terrain_type::GREEN_KEY;

    /* row 21 */
    for (uint32_t xx = 12; xx < 19; ++xx)
    {
        the_map[Grid::Location{.x = xx, .y = 21}] = terrain_type::WALL;
    }
}

void State::move_entities() noexcept
{
    for (auto &ent : entity_list)
    {

        /*  If this guy is dead, or timer hasn't expired yet for a move, then skip.
            Dead entities are cleaned up later.
        */
        if (!ent.alive || absolute_time_diff_us(get_absolute_time(), ent.next_time) > 0)
        {
            continue;
        }

        /* Update next move time w/o drift */
        ent.next_time = delayed_by_us(ent.next_time, get_entity_velocity(ent.identity));

        /* get the function table for this entity */
        const auto entity_handles{get_state_functions(ent.identity)};

        /* begin processing with the entry handler */
        if (entity_handles.entry_handler != nullptr)
        {
            entity_handles.entry_handler(ent);
        }

        /* next, compute where this entity wants to move */
        const auto [nextloc, nextfacing]{entity_handles.process_move_handler != nullptr
                                             ? entity_handles.process_move_handler(ent)
                                             : std::make_pair(ent.loc, ent.facing)};

        if (play_grid.out_of_bounds(nextloc))
        {
            ent.alive = false;
            continue;
        }

        /* At least for now, we always update the facing, even if we don't end up moving.
           note that the collision handler may further edit the facing. */
        ent.facing = nextfacing;

        /* if the next location is the same as the current, then there's nothing more to do */
        if (nextloc == ent.loc)
        {
            continue;
        }

        /* then, determine and resolve collisions */
        const collision_result collision{find_collisions(nextloc)};

        const collision_action action{
            entity_handles.entity_collision_handler != nullptr
                ? entity_handles.entity_collision_handler(ent, collision.other, collision.tile)
                : collision_action::NO_ACTION_NEEDED};

        /* resolve side-effects from collision handler code */
        if (action == collision_action::NO_ACTION_NEEDED)
        {
            continue;
        }

        if (action == collision_action::MARK_DEAD)
        {
            ent.alive = false;
            continue;
        }

        /* TODO feed this with the tile from the previous entity location */
        // apply_terrain_exit_effect(ent, collision.tile);

        if (action == collision_action::APPLY_NEXT_LOCATION)
        {
            ent.loc = nextloc;
        }
        else
        {
            /* made it this far?  We can attempt the move */
            const auto direction_to_move{[&]() {
                if (action == collision_action::MOVE_FORWARD)
                {
                    return relative_direction::FORWARD;
                }
                if (action == collision_action::MOVE_BACKWARD)
                {
                    return relative_direction::BACKWARD;
                }
                if (action == collision_action::MOVE_RIGHT)
                {
                    return relative_direction::RIGHT;
                }
                if (action == collision_action::MOVE_LEFT)
                {
                    return relative_direction::LEFT;
                }
                return relative_direction::FORWARD;
            }()};

            /* then, effect any terrain */
            move_relative_direction(ent, direction_to_move);
        }

        apply_terrain_entry_effect(ent, collision.tile);

        /* finally, process the exit handler */
        if (entity_handles.exit_handler != nullptr)
        {
            entity_handles.exit_handler(ent);
        }
    }
}

[[nodiscard]] collision_result State::find_collisions(Grid::Location next_location) noexcept
{
    collision_result result{.other = nullptr, .tile = the_map[next_location]};

    /* search for other entities */
    auto entitr = std::find_if(std::begin(entity_list), std::end(entity_list),
                               [&](const auto &other) { return other.loc == next_location; });
    if (entitr != std::end(entity_list))
    {
        result.other = entitr;
    }

    return result;
}

void State::update_view_port_position() noexcept
{
    /* view port is always centered on Chippie, within the bounds of the play field */
    const auto [chipx, chipy]{entity_list.front().loc};
    view_port.x = chipx < 4 ? chipx : chipx - 4;
    view_port.y = chipy < 4 ? chipy : chipy - 4;
}

[[nodiscard]] bool State::within_view_port(Grid::Location loc) noexcept
{
    return view_port.contains(loc.x, loc.y);
}

void State::paint_the_map() noexcept
{
    /* first, update the view_port */
    update_view_port_position();

    /* next, draw the view port on the physical screen */
    for (uint32_t yy = 0; yy < 9; ++yy)
    {
        for (uint32_t xx = 0; xx < 9; ++xx)
        {
            const Grid::Location abs_loc{.x = xx + view_port.x, .y = yy + view_port.y};
            const terrain_type terrain{the_map[abs_loc]};
            draw_tile_index(view_grid, make_relative_to_view_port(abs_loc), texture::get_terrain_texture_type(terrain));
        }
    }
}
void State::paint_chip_count_display() noexcept
{
}
void State::paint_timer_display() noexcept
{
}
void State::paint_level_display() noexcept
{
}

void State::paint_entity(const entity &ent) const noexcept
{
    const auto loc{make_relative_to_view_port(ent.loc)};
    draw_tile_index(view_grid, loc, texture::get_entity_texture_type(ent));
}

[[nodiscard]] Grid::Location State::make_relative_to_view_port(Grid::Location abs_loc) const noexcept
{
    return {.x = abs_loc.x - view_port.x, .y = abs_loc.y - view_port.y};
}

void State::init_play_grid() noexcept
{
    const auto [width, height]{screen::get_virtual_screen_size()};
    const Grid::GridCfg view_cfg{.xdimension = {.off = (width - TILE_SIDE_LENGTH * 9) / 2, .scale = TILE_SIDE_LENGTH},
                                 .ydimension = {.off = 10, .scale = TILE_SIDE_LENGTH},
                                 .grid_width = 9,
                                 .grid_height = 9};
    view_grid = Grid{view_cfg};

    static constexpr Grid::GridCfg play_cfg{.xdimension = {.off = 0, .scale = 1},
                                 .ydimension = {.off = 0, .scale = 1},
                                 .grid_width = 32,
                                 .grid_height = 32};
    play_grid = Grid{play_cfg};
}

} // namespace chippie
