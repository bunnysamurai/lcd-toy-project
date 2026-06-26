#include "chippie/entities/chippie_entity.hpp"
#include "chippie/events/event.hpp"
#include "chippie/textures/texture_defs.hpp"

#include "state/game_logic.hpp"
#include "state/state.hpp"

#include "gamepad/gamepad.hpp"
#include "screen/gfx/dialog.hpp"
#include "screen/glyphs/letters.hpp"

#include "pico/time.h"

#include <cstring>

// #define DEBUG_PRINT
#ifdef DEBUG_PRINT
#include <pico/printf.h>
#endif

namespace chippie
{
namespace
{

constexpr int PAINT_TIME_INTERVAL_US{33'333};

void screen_init() noexcept
{
    /* we are a color application */
    screen::set_format(TILE_SCREEN_FORMAT);
    screen::init_clut(Palette.data(), Palette.size());

    /* we use a light grey background */
    screen::fill_screen((LGREY << 4) | LGREY);

    /* we use the dialog, so let's set its palette */
    screen::gfx::set_dialog_palette({
        .bright_highlight = WHITE,
        .shadow_highlight = DRKGRY,
        .background = BLACK,
        .font_color = YELLOW,
    });
}

void load_level_from_rom_stub(state &game_state, uint8_t level) noexcept
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

void load_entity_list_from_rom_stub(state &game_state, [[maybe_unused]] uint8_t level) noexcept
{
    game_state.entity_list.clear();
    // by convention, Chippie is always the first one in the entity_list
    game_state.entity_list.push_back(chippie::create(game_state, {.x = 15, .y = 14}, direction::DOWN, 0));
}

[[nodiscard]] bool load_level(state &game_state, int level_number) noexcept
{
    if (level_number != 1)
    {
        return false;
    }

    load_level_from_rom_stub(game_state, level_number);
    load_entity_list_from_rom_stub(game_state, level_number);

    return true;
}

void init_event_handlers() noexcept
{
    event::register_handler(event::event_handler{
        .identifier = event::event_type::TIME_UP,
        .handler =
            [](state &game_state) {
                game_state.active = false;
                game_state.inactive_reason = state::reason::CHIP_DIED;
            },
    });
    event::register_handler(event::event_handler{
        .identifier = event::event_type::NEXT_LEVEL,
        .handler =
            [](state &game_state) {
                game_state.active = false;
                game_state.inactive_reason = state::reason::PORTAL_REACHED;
            },
    });
    event::register_handler(event::event_handler{
        .identifier = event::event_type::DISPLAY_HINT,
        .handler = [](state &game_state) { game_state.hint_displayable = true; },
    });
    event::register_handler(event::event_handler{
        .identifier = event::event_type::CLEAR_HINT,
        .handler = [](state &game_state) { game_state.hint_displayable = false; },
    });
    event::register_handler(event::event_handler{
        .identifier = event::event_type::OPEN_MENU,
        .handler =
            [](state &game_state) {
                game_state.active = false;
                game_state.inactive_reason = state::reason::USER_QUIT;
            },
    });
}

void init_play_grid(state &game_state) noexcept
{
    const auto [width, height]{screen::get_virtual_screen_size()};
    const Grid::GridCfg view_cfg{
        .xdimension = {.off = (width - TILE_SIDE_LENGTH * 9) / 2, .scale = TILE_SIDE_LENGTH},
        .ydimension = {.off = 10, .scale = TILE_SIDE_LENGTH},
        .grid_width = 9,
        .grid_height = 9,
    };
    game_state.view_grid = Grid{view_cfg};

    static constexpr Grid::GridCfg play_cfg{
        .xdimension = {.off = 0, .scale = 1},
        .ydimension = {.off = 0, .scale = 1},
        .grid_width = 32,
        .grid_height = 32,
    };
    game_state.play_grid = Grid{play_cfg};

    const auto interval{(width - TILE_SIDE_LENGTH * 3 * 3) / 4};
    const Grid::GridCfg level_count_cfg{
        .xdimension = {.off = interval, .scale = TILE_SIDE_LENGTH},
        .ydimension = {.off = 10 + 9 * TILE_SIDE_LENGTH + 10, .scale = TILE_SIDE_LENGTH},
        .grid_width = 3,
        .grid_height = 1,
    };
    const Grid::GridCfg time_remaining_cfg{
        .xdimension = {.off = 2 * interval + 3 * TILE_SIDE_LENGTH, .scale = TILE_SIDE_LENGTH},
        .ydimension = {.off = 10 + 9 * TILE_SIDE_LENGTH + 10, .scale = TILE_SIDE_LENGTH},
        .grid_width = 3,
        .grid_height = 1,
    };
    const Grid::GridCfg chip_count_cfg{
        .xdimension = {.off = 3 * interval + 2 * 3 * TILE_SIDE_LENGTH, .scale = TILE_SIDE_LENGTH},
        .ydimension = {.off = 10 + 9 * TILE_SIDE_LENGTH + 10, .scale = TILE_SIDE_LENGTH},
        .grid_width = 3,
        .grid_height = 1,
    };

    game_state.chip_count_grid = Grid{chip_count_cfg};
    game_state.level_count_grid = Grid{level_count_cfg};
    game_state.time_remaining_grid = Grid{time_remaining_cfg};

    /* hint area is the portion of the screen where we'll print the hint text.  It lives in the space between the bottom
     * of the view grid and the bottom of the physical screen */
    game_state.hint_area.topleft.x = level_count_cfg.xdimension.off;
    game_state.hint_area.topleft.y = level_count_cfg.ydimension.off;
    game_state.hint_area.size.width = width - 2 * game_state.hint_area.topleft.x;
    const auto bottom_row_of_view_area{view_cfg.grid_height * view_cfg.ydimension.scale + view_cfg.ydimension.off};
    const auto total_height_available{height - bottom_row_of_view_area};
    const auto height_after_applying_margin{
        total_height_available - (game_state.hint_area.topleft.y - bottom_row_of_view_area) - view_cfg.ydimension.off};
    game_state.hint_area.size.height = height_after_applying_margin;

    const Grid::GridCfg hint_print_grid_cfg{
        .xdimension = {.off = game_state.hint_area.topleft.x, .scale = glyphs::tile::width()},
        .ydimension = {.off = game_state.hint_area.topleft.y, .scale = glyphs::tile::height()},
        .grid_width = game_state.hint_area.size.width / glyphs::tile::width(),
        .grid_height = game_state.hint_area.size.height / glyphs::tile::height(),
    };
    game_state.hint_print_grid = Grid{hint_print_grid_cfg};

    /* inventory grid fills the final gap between the three counters and the bottom of the screen */
    const auto height_for_inventory_grid{height_after_applying_margin - TILE_SIDE_LENGTH};
    const auto bottom_row_of_chip_count{chip_count_cfg.ydimension.off +
                                        chip_count_cfg.ydimension.scale * chip_count_cfg.grid_height};
    const Grid::GridCfg inventory_cfg{
        .xdimension = {.off = width / 2 - 4 * TILE_SIDE_LENGTH, .scale = TILE_SIDE_LENGTH},
        .ydimension = {.off = bottom_row_of_chip_count + height_for_inventory_grid / 2 - TILE_SIDE_LENGTH / 2,
                       .scale = TILE_SIDE_LENGTH},
        .grid_width = 8,
        .grid_height = 1,
    };
    game_state.inventory_grid = Grid{inventory_cfg};

#ifdef DEBUG_PRINT
    printf("hint area\n");
    printf("    x      %d\n", game_state.hint_area.topleft.x);
    printf("    y      %d\n", game_state.hint_area.topleft.y);
    printf("    width  %d\n", game_state.hint_area.size.width);
    printf("    height %d\n", game_state.hint_area.size.height);
#endif
}

void process_opening_level_dialog(state &game_state) noexcept
{
    /* draw the level dialog */
    screen::gfx::display_dialog_box(game_state.level_name_text);

    /* spin until the user presses a button */
    while (true)
    {
        const auto [up, down, right, left, etc, unused]{gamepad::five::get()};
        const bool result = up | down | right | left | etc;

        if (result)
        {
            break;
        }

        /* TODO pico-sdk specific call should be abstracted for better portability */
        sleep_ms(1);
    }

    game_state.countdown_timer.reset();
    game_state.last_paint_time = get_absolute_time();
}

} // namespace

void run()
{
    gamepad::five::init();
    screen_init();

    /* menu should go here */
    // const auto result{menu.run()};

    int level = 1;

    while (true)
    {
        state game_state;
#ifdef DEBUG_PRINT
        printf("top of main loop\n");
#endif
        /* state init */
        init_event_handlers();
        init_play_grid(game_state);

        if (!load_level(game_state, level))
        {
            /* we are out of levels.  Exit to menu (or in this case, simply quit the game) */
            break;
        }

        paint(game_state);

        process_opening_level_dialog(game_state);

        game_logic the_game{game_state};
#ifdef DEBUG_PRINT
        printf("game logic intialized\n");
#endif

        static constexpr int64_t GAME_LOOP_INTERVAL_US{1'000};

        const auto beginning_of_game{get_absolute_time()};

        /* TODO instead, create an object of class GameLogic that is initalized with the State
          This GameLogic object will more-or-less replace the `state` object used in the next
          statements
          GameLogic will do much of the logic that is going on inside State at the moment, except
          the entity handling and event handlers will depend on the State object being passed to it.
        */
        while (the_game.is_active())
        {
#ifdef DEBUG_PRINT
            printf("top of game logic loop\n");
#endif
            const auto start{get_absolute_time()};

            the_game.process();

            /* paint the entire frame every 33 ms */
            if (absolute_time_diff_us(game_state.last_paint_time, get_absolute_time()) > PAINT_TIME_INTERVAL_US)
            {
                game_state.last_paint_time = delayed_by_us(game_state.last_paint_time, PAINT_TIME_INTERVAL_US);
                paint(game_state);
            }

            const auto process_duration{absolute_time_diff_us(start, get_absolute_time())};
#ifdef DEBUG_PRINT
            printf("[%09lld] Process duration: %lld us\n", absolute_time_diff_us(start, get_absolute_time()),
                   process_duration);
#endif
            if (process_duration < GAME_LOOP_INTERVAL_US)
            {
#ifdef DEBUG_PRINT
                printf("[%09lld] Delaying for: %lld us\n", absolute_time_diff_us(start, get_absolute_time()),
                       GAME_LOOP_INTERVAL_US - process_duration);
#endif
                sleep_us(GAME_LOOP_INTERVAL_US - process_duration);
            }

#ifdef DEBUG_PRINT
            printf("bottom of game logic loop\n");
#endif
        }

        /* TODO add logic for an "end of game", maybe go back to a menu?
            also logic for why the game isn't active:
                if chip died, the level should restart
                if portal was reached, the level number should increment
                if a quit was requested, break out of the outer loop
        */

        bool leave_loop{false};

        switch (game_state.inactive_reason)
        {
        case state::reason::CHIP_DIED:
            break;
        case state::reason::PORTAL_REACHED:
            ++level;
            break;
        case state::reason::USER_QUIT:
            leave_loop = true;
            break;
        }

        if (leave_loop)
        {
            /* TODO maybe go to a menu, instead?  The menu should allow the user to view scores,
               select an already beaten level, jump to a level via the password, and quit the game */
            break;
        }
    }

    gamepad::five::deinit();
}

} // namespace chippie