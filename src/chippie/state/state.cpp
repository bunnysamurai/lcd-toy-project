#include "state.hpp"

#include "chippie/render/paint_utils.hpp"
#include "chippie/textures/texture.hpp"
#include "chippie/textures/texture_defs.hpp"

#include "screen/gfx/dialog.hpp"
#include "screen/gfx/shapes.hpp"
#include "screen/glyphs/letter_utils.hpp"
#include "screen/glyphs/letters.hpp"
#include "screen/screen.hpp"

#include "common/screen_utils.hpp"

#include <array>

// #define DEBUG_PRINT
#ifdef DEBUG_PRINT
#include <pico/printf.h>
#endif

namespace chippie
{
namespace
{

/*===========================================================*/
/*
 _   _ _   _ _
| | | | |_(_) |___
| | | | __| | / __|
| |_| | |_| | \__ \
 \___/ \__|_|_|___/

*/
/*===========================================================*/

[[nodiscard]] Grid::Location make_relative_to_view_port(const state &game_state, Grid::Location abs_loc) noexcept
{
    return {.x = abs_loc.x - game_state.view_port.topleft.x, .y = abs_loc.y - game_state.view_port.topleft.y};
}

void update_view_port_position(state &game_state) noexcept
{
    /* view port is always centered on Chippie, within the bounds of the play field */
    const auto [chipx, chipy]{game_state.entity_list.front().loc};

    /* compute x location */
    if (chipx < 4U)
    {
        game_state.view_port.topleft.x = 0;
    }
    else if (chipx + 4U >= game_state.play_grid.config().grid_width)
    {
        game_state.view_port.topleft.x = game_state.play_grid.config().grid_width - game_state.view_port.size.width;
    }
    else
    {
        game_state.view_port.topleft.x = chipx - 4U;
    }

    /* again, for y */
    if (chipy < 4U)
    {
        game_state.view_port.topleft.y = 0;
    }
    else if (chipy + 4U >= game_state.play_grid.config().grid_height)
    {
        game_state.view_port.topleft.y = game_state.play_grid.config().grid_height - game_state.view_port.size.height;
    }
    else
    {
        game_state.view_port.topleft.y = chipy - 4U;
    }
}

[[nodiscard]] bool within_view_port(const state &game_state, Grid::Location loc) noexcept
{
    return game_state.view_port.contains({.x = loc.x, .y = loc.y});
}

/*===========================================================*/
/*
    _        _               _   ____       _       _   _
   / \   ___| |_ _   _  __ _| | |  _ \ __ _(_)_ __ | |_(_)_ __   __ _
  / _ \ / __| __| | | |/ _` | | | |_) / _` | | '_ \| __| | '_ \ / _` |
 / ___ \ (__| |_| |_| | (_| | | |  __/ (_| | | | | | |_| | | | | (_| |
/_/   \_\___|\__|\__,_|\__,_|_| |_|   \__,_|_|_| |_|\__|_|_| |_|\__, |
                                                                |___/
*/
/*===========================================================*/
void paint_the_background() noexcept
{
    /* we use a light grey background */
    screen::fill_screen((LGREY << 4) | LGREY);
}

void paint_the_map(state &game_state) noexcept
{
    /* first, update the view_port */
    /* TODO should this function be here, or move somewhere else? */
    update_view_port_position(game_state);

    /* next, draw the view port on the physical screen */
    for (uint32_t yy = 0; yy < game_state.view_port.size.height; ++yy)
    {
        for (uint32_t xx = 0; xx < game_state.view_port.size.width; ++xx)
        {
            const Grid::Location abs_loc{.x = xx + game_state.view_port.topleft.x,
                                         .y = yy + game_state.view_port.topleft.y};
            const terrain_type terrain{game_state.the_map[abs_loc]};
            draw_tile_index(game_state.view_grid, make_relative_to_view_port(game_state, abs_loc),
                            texture::get_terrain_texture_type(terrain));
        }
    }
}

void paint_chip_count_display(const state &game_state) noexcept
{
    const auto chip_count{game_state.chippie_inventory.check(inventory_item::CHIPS)};
    const auto [msb, mmb, lsb]{screen::bcd<3>(chip_count)};

    if (!msb && !mmb && !lsb)
    {
        draw_tile_index(game_state.chip_count_grid, {.x = 0, .y = 0},
                        texture::get_texture(texture::texture_type::DIGIT_00_YELLOW));
        draw_tile_index(game_state.chip_count_grid, {.x = 1, .y = 0},
                        texture::get_texture(texture::texture_type::DIGIT_00_YELLOW));
        draw_tile_index(game_state.chip_count_grid, {.x = 2, .y = 0},
                        texture::get_texture(texture::texture_type::DIGIT_00_YELLOW));
    }
    else
    {
        draw_tile_index(game_state.chip_count_grid, {.x = 0, .y = 0}, texture::get_digit_texture(msb));
        draw_tile_index(game_state.chip_count_grid, {.x = 1, .y = 0}, texture::get_digit_texture(mmb));
        draw_tile_index(game_state.chip_count_grid, {.x = 2, .y = 0}, texture::get_digit_texture(lsb));
    }
}

void paint_timer_display(const state &game_state) noexcept
{
    const auto [msb, mmb, lsb]{screen::bcd<3>(game_state.time_remaining)};

    draw_tile_index(game_state.time_remaining_grid, {.x = 0, .y = 0}, texture::get_digit_texture(msb));
    draw_tile_index(game_state.time_remaining_grid, {.x = 1, .y = 0}, texture::get_digit_texture(mmb));
    draw_tile_index(game_state.time_remaining_grid, {.x = 2, .y = 0}, texture::get_digit_texture(lsb));
}

void paint_level_display(const state &game_state) noexcept
{
    const auto [msb, mmb, lsb]{screen::bcd<3>(game_state.level_number)};

    draw_tile_index(game_state.level_count_grid, {.x = 0, .y = 0}, texture::get_digit_texture(msb));
    draw_tile_index(game_state.level_count_grid, {.x = 1, .y = 0}, texture::get_digit_texture(mmb));
    draw_tile_index(game_state.level_count_grid, {.x = 2, .y = 0}, texture::get_digit_texture(lsb));
}

void paint_hint(const state &game_state) noexcept
{
    screen::gfx::draw_rect(game_state.hint_area, BLACK, 0);

    /* draw the letters out one by one */
    if (!game_state.hint_text)
    {
        return;
    }

    int start_y{1};
    int start_x{1};
    int ii{};
    while (game_state.hint_text[ii] != '\0')
    {
        const auto [outx, outy]{game_state.hint_print_grid.to_native({.x = start_x, .y = start_y})};

        screen::draw_standard_character_to_4bpp_display(game_state.hint_text[ii], outx, outy, WHITE, BLACK);

        /* then, increment to the next column or do a carriage return if at the end of the line */
        if (start_x + 1 < game_state.hint_print_grid.config().grid_width - 1)
        {
            ++start_x;
        }
        else
        {
            start_x = 1;
            ++start_y;
        }
        ++ii;
    }
}

void paint_inventory(const state &game_state) noexcept
{
    /* order is boots, suckers, skates, flippers, red, cyan, yellow, green */

    int xloc{0};

    static constexpr std::array inventory_items{
        inventory_item::FIRE_BOOTS, inventory_item::SUCTION_BOOTS, inventory_item::ICE_SKATES,
        inventory_item::FLIPPERS,   inventory_item::RED_KEY,       inventory_item::CYAN_KEY,
        inventory_item::YELLOW_KEY, inventory_item::GREEN_KEY,
    };

    static constexpr std::array item_textures{
        texture::texture_type::FIRE_BOOTS, texture::texture_type::SUCTION_BOOTS, texture::texture_type::ICE_SKATES,
        texture::texture_type::FLIPPERS,   texture::texture_type::RED_KEY,       texture::texture_type::CYAN_KEY,
        texture::texture_type::YELLOW_KEY, texture::texture_type::GREEN_KEY,
    };

    const auto clear_type{texture::get_texture(texture::texture_type::CLEAR)};

    for (int xloc = 0; xloc < std::size(inventory_items); ++xloc)
    {

        draw_tile_index(game_state.inventory_grid, {.x = xloc, .y = 0},
                        game_state.chippie_inventory.check(inventory_items[xloc])
                            ? texture::get_texture(item_textures[xloc])
                            : clear_type);
    }
}

void paint_entity(const state &game_state, const entity &ent) noexcept
{
    const auto loc{make_relative_to_view_port(game_state, ent.loc)};
    draw_tile_index(game_state.view_grid, loc, texture::get_entity_texture_type(ent));
}

} // namespace

/*===========================================================*/
/*
 ____        _     _ _
|  _ \ _   _| |__ | (_) ___
| |_) | | | | '_ \| | |/ __|
|  __/| |_| | |_) | | | (__
|_|    \__,_|_.__/|_|_|\___|

*/
/*===========================================================*/

void paint(state &game_state) noexcept
{

#ifdef DEBUG_PRINT
    printf("painting game state\n");
#endif
    screen::pause_screen();

    paint_the_background();

    paint_the_map(game_state);

    if (!game_state.hint_displayable)
    {
        paint_chip_count_display(game_state);
        paint_timer_display(game_state);
        paint_level_display(game_state);
        paint_inventory(game_state);
    }
    else
    {
        paint_hint(game_state);
    }

    for (const auto &ent : game_state.entity_list)
    {
        if (within_view_port(game_state, ent.loc))
        {
            paint_entity(game_state, ent);
        }
    }

    screen::resume_screen();
}

} // namespace chippie