#if !defined(EVENT_HPP)
#define EVENT_HPP

#include "chippie/textures/texture_defs.hpp"
#include "state/state.hpp"

#include "gamepad/gamepad.hpp"

#include "pico/time.h"

// #define DEBUG_PRINT
#ifdef DEBUG_PRINT
#include <pico/printf.h>
#endif

namespace chippie
{

void screen_init() noexcept
{
    /* we are a color application */
    screen::set_format(chippie::TILE_SCREEN_FORMAT);
    screen::init_clut(chippie::Palette.data(), chippie::Palette.size());

    /* we use a light grey background */
    screen::fill_screen((LGREY << 4) | LGREY);
}

void run()
{
    gamepad::five::init();
    screen_init();

    /* menu should go here */
    // const auto result{menu.run()};

    State state;

    /* load the "level" */
    state.load_level();

    static constexpr int64_t GAME_LOOP_INTERVAL_US{1'000};

    const auto beginning_of_game{get_absolute_time()};

    while (state.is_active())
    {
        const auto start{get_absolute_time()};

        state.process();

        const auto process_duration{absolute_time_diff_us(start, get_absolute_time())};
#ifdef DEBUG_PRINT
        printf("[%09lld] Process duration: %lld us\n", absolute_time_diff_us(start, get_absolute_time()), process_duration);
#endif
        if (process_duration < GAME_LOOP_INTERVAL_US)
        {
#ifdef DEBUG_PRINT
            printf("[%09lld] Delaying for: %lld us\n", absolute_time_diff_us(start, get_absolute_time()),
                   GAME_LOOP_INTERVAL_US - process_duration);
#endif
            sleep_us(GAME_LOOP_INTERVAL_US - process_duration);
        }
    }

    /* TODO add logic for an "end of game", maybe go back to a menu? */

    gamepad::five::deinit();
}

} // namespace chippie
#endif