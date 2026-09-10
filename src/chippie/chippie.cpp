#include "chippie/chippie_common.hpp"
#include "chippie/entities/basic_entity.hpp"
#include "chippie/entities/chippie_entity.hpp"
#include "chippie/events/event.hpp"
#include "chippie/textures/texture_defs.hpp"

#include "state/game_logic.hpp"
#include "state/state.hpp"

#include "chippie/levels/level.hpp"

#include "common/rng.hpp"
#include "gamepad/gamepad.hpp"
#include "screen/gfx/cursor_menu_dialog.hpp"
#include "screen/gfx/dialog.hpp"
#include "screen/glyphs/letters.hpp"

#include "pico/time.h"

#include <array>
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
constexpr int64_t GAME_LOOP_INTERVAL_US{1'000};

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

[[nodiscard]] bool load_level(state &game_state, int level_number) noexcept
{
    if (level_number > level::get_max_level())
    {
        return false;
    }

    level::load(game_state, level_number);
    game_state.level_number = level_number;

    return true;
}

void init_event_handlers() noexcept
{
    event::register_handler(event::event_handler{
        .identifier = event::event_type::SMUSHED,
        .handler =
            [](state &game_state) {
                game_state.active = false;
                game_state.inactive_reason = state::reason::CHIP_DIED;
            },
    });
    event::register_handler(event::event_handler{
        .identifier = event::event_type::CHOMPED_BY_FROG,
        .handler =
            [](state &game_state) {
                game_state.active = false;
                game_state.inactive_reason = state::reason::CHIP_DIED;
            },
    });
    event::register_handler(event::event_handler{
        .identifier = event::event_type::ROLLED_BY_BALL,
        .handler =
            [](state &game_state) {
                game_state.active = false;
                game_state.inactive_reason = state::reason::CHIP_DIED;
            },
    });
    event::register_handler(event::event_handler{
        .identifier = event::event_type::GLIDED_OVER,
        .handler =
            [](state &game_state) {
                game_state.active = false;
                game_state.inactive_reason = state::reason::CHIP_DIED;
            },
    });
    event::register_handler(event::event_handler{
        .identifier = event::event_type::DANCED_BY_FIRE,
        .handler =
            [](state &game_state) {
                game_state.active = false;
                game_state.inactive_reason = state::reason::CHIP_DIED;
            },
    });
    event::register_handler(event::event_handler{
        .identifier = event::event_type::EXPLODED,
        .handler =
            [](state &game_state) {
                game_state.active = false;
                game_state.inactive_reason = state::reason::CHIP_DIED;
            },
    });
    event::register_handler(event::event_handler{
        .identifier = event::event_type::FLATTENED_BY_TANK,
        .handler =
            [](state &game_state) {
                game_state.active = false;
                game_state.inactive_reason = state::reason::CHIP_DIED;
            },
    });
    event::register_handler(event::event_handler{
        .identifier = event::event_type::GOT_BURNED,
        .handler =
            [](state &game_state) {
                game_state.active = false;
                game_state.inactive_reason = state::reason::CHIP_DIED;
            },
    });
    event::register_handler(event::event_handler{
        .identifier = event::event_type::EATEN_BY_BUG,
        .handler =
            [](state &game_state) {
                game_state.active = false;
                game_state.inactive_reason = state::reason::CHIP_DIED;
            },
    });
    event::register_handler(event::event_handler{
        .identifier = event::event_type::FELL_IN_WATER,
        .handler =
            [](state &game_state) {
                game_state.active = false;
                game_state.inactive_reason = state::reason::CHIP_DIED;
            },
    });
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
                /*
                  TODO "pause" the clock that the game logic uses somehow
                  so that if the user selects "Return", the game logic processing will
                  pick up right where it left off...
                 */
                static constexpr std::array item_options{
                    "Return",
                    "Restart",
                    "Quit",
                };
                screen::gfx::cursor_menu_dialog dialog{
                    {.bright_highlight = WHITE, .shadow_highlight = DRKGRY, .background = LGREY, .font_color = WHITE},
                    "   Menu",
                    item_options};

                game_state.the_clock.pause();

                const int opt{dialog.ask()};

                if (opt == 1)
                {
                    game_state.active = false;
                    game_state.inactive_reason = state::reason::RESTART_LEVEL;
                }

                if (opt == 2)
                {
                    game_state.active = false;
                    game_state.inactive_reason = state::reason::USER_QUIT;
                }

                game_state.the_clock.resume();
            },
    });
    event::register_handler(event::event_handler{
        .identifier = event::event_type::GREEN_BUTTON,
        .handler =
            [](state &game_state) {
                /* search throught the map, inverting any green button clear/wall tiles */
                for (auto &tile : game_state.the_map.map_data)
                {
                    switch (tile)
                    {
                    case terrain_type::GREEN_BUTTON_CLEAR:
                        tile = terrain_type::GREEN_BUTTON_WALL;
                        break;
                    case terrain_type::GREEN_BUTTON_WALL:
                        tile = terrain_type::GREEN_BUTTON_CLEAR;
                    default:
                        break;
                    }
                }
            },
    });
    event::register_handler(event::event_handler{
        .identifier = event::event_type::BLUE_BUTTON,
        .handler =
            [](state &game_state) {
                /* search throught the entity list, inverting any blue tanks */
                for (auto &ent : game_state.entity_list)
                {
                    switch (ent.identity)
                    {
                    case entity_type::BLUE_TANK_THAT_MOVES_DOWN:
                        ent.identity = entity_type::BLUE_TANK_THAT_MOVES_UP;
                        break;
                    case entity_type::BLUE_TANK_THAT_MOVES_UP:
                        ent.identity = entity_type::BLUE_TANK_THAT_MOVES_DOWN;
                        break;
                    case entity_type::BLUE_TANK_THAT_MOVES_LEFT:
                        ent.identity = entity_type::BLUE_TANK_THAT_MOVES_RIGHT;
                        break;
                    case entity_type::BLUE_TANK_THAT_MOVES_RIGHT:
                        ent.identity = entity_type::BLUE_TANK_THAT_MOVES_LEFT;
                        break;
                    default:
                        break;
                    }
                }
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

    /* FIXME hack, wait for user to release button instead? */
    sleep_ms(1000);

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
    game_state.last_paint_time = steady_clock_source::now();
}

void process_screen_painting(state &game_state) noexcept
{
    const auto now_time_for_painting{steady_clock_source::now()};
    if (steady_clock_source::time_diff(game_state.last_paint_time, now_time_for_painting) > PAINT_TIME_INTERVAL_US)
    {
        /* calls to the_game.process() may take arbitrary long amount of time...
           guarentee the next time we paint will be in the future */
        while (steady_clock_source::time_diff(game_state.last_paint_time, now_time_for_painting) >
               PAINT_TIME_INTERVAL_US)
        {
            game_state.last_paint_time =
                steady_clock_source::increment_time_point(game_state.last_paint_time, PAINT_TIME_INTERVAL_US);
        }
        paint(game_state);
    }
}

void sleep_until_next_game_loop_iteration(const steady_clock_source::time_base_t start) noexcept
{
    const auto process_duration{steady_clock_source::time_diff(start, steady_clock_source::now())};
#ifdef DEBUG_PRINT
            printf("[%09lld] Process duration: %lld us\n", steady_clock_source::time_diff(start, steady_clock_source::now())),
                   process_duration);
#endif
            if (process_duration < GAME_LOOP_INTERVAL_US)
            {
#ifdef DEBUG_PRINT
                printf("[%09lld] Delaying for: %lld us\n",
                       steady_clock_source::time_diff(start, steady_clock_source::now()),
                       GAME_LOOP_INTERVAL_US - process_duration);
#endif
                sleep_us(GAME_LOOP_INTERVAL_US - process_duration);
            }
}
} // namespace

void run()
{
    gamepad::five::init();
    screen_init();
    rng::set_seed(0xB5FF93DC);

    /* menu should go here */
    // const auto result{menu.run()};

    const int MAX_LEVELS = level::get_max_level();
    int level = 1; /* the starting level */

    while (true)
    {
        state game_state;

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

        while (the_game.is_active())
        {
            const auto start{steady_clock_source::now()};

            the_game.process();

            /* paint the entire frame on a periodic rate, different from the game loop */
            process_screen_painting(game_state);

            sleep_until_next_game_loop_iteration(start);
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
        case state::reason::RESTART_LEVEL:
        case state::reason::CHIP_DIED:
            break;
        case state::reason::PORTAL_REACHED:
            level = (level == MAX_LEVELS) ? 1 : (level + 1);
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
