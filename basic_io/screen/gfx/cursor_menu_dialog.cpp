#include "cursor_menu_dialog.hpp"

#include <cstdint>
#include <span>

// #include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <numeric>
#include <utility>

#include "pico/time.h"

#include "../../gamepad/gamepad.hpp"
#include "../../utils/Cursor.hpp"
#include "../glyphs/letter_utils.hpp"
#include "../glyphs/letters.hpp"
#include "details/dialog_utils.hpp"

#include "defs.hpp"
#include "shapes.hpp"

namespace screen::gfx
{
namespace
{

enum struct UserInstruction
{
    NOACTION,
    CURSOR_UP,
    CURSOR_DOWN,
    LAUNCH_PROGRAM
};

struct MenuConfig
{
    uint32_t startcol;
    uint32_t startline;
    uint32_t row_spacing;
    uint32_t titlestartline;
};

static const auto GRID_WIDTH{glyphs::tile::width()};
static const auto GRID_HEIGHT{glyphs::tile::height()};

MenuConfig g_cfg;

void init_menu_cfg() noexcept
{
    g_cfg.startcol = 1;
    g_cfg.startline = 2;
    g_cfg.row_spacing = 1;
    g_cfg.titlestartline = 1;
}

[[nodiscard]] UserInstruction process_user_input() noexcept
{
    UserInstruction result{UserInstruction::NOACTION};
    auto state{gamepad::five::get()};

    if (state.up)
    {
        result = UserInstruction::CURSOR_UP;
    }
    if (state.down)
    {
        result = UserInstruction::CURSOR_DOWN;
    }
    if (state.etc)
    {
        result = UserInstruction::LAUNCH_PROGRAM;
    }

    if (result == UserInstruction::NOACTION)
    {
        return result;
    }

    /* in case a button is still pressed, spin until they are all clear */
    bool triggered{false};
    for (;;)
    {
        state = gamepad::five::get();

        uint8_t data;
        std::memcpy(&data, &state, sizeof(state));
        if (!data && triggered == false)
        {
            triggered = true;
        }
        else if (data && triggered == true)
        {
            triggered = false;
        }
        else if (!data && triggered == true)
        {
            break;
        }
        sleep_ms(10);
    }

    return result;
}

} // namespace

/*
    Need to specify how many options and strings to describe those options,
    When "ask" is called, does the following:
        draws the menu on the screen
        processes user input
        when user "selects", returns the index in the menu that was selected
*/
cursor_menu_dialog::cursor_menu_dialog(cursor_menu_dialog_palette palette, const char *title,
                                       std::span<const char *const> items) noexcept
    : m_palette{palette}, m_items{items}, m_title{title}, m_cursor{std::size(items)}
{
    init_menu_cfg();
}

int cursor_menu_dialog::ask() noexcept
{
    /* draw the menu, which is a rectangle plus a border */
    m_menu_rect = compute_background_shape();

    screen::pause_screen();
    draw_rect(m_menu_rect, m_palette.background, 0);
    draw_border(m_menu_rect, 4, m_palette.bright_highlight, m_palette.shadow_highlight);
    screen::resume_screen();

    /* run the menu logic */
    return process_menu();
}

[[nodiscard]] Rect cursor_menu_dialog::compute_background_shape() const noexcept
{
    /* find the longest length string */
    const auto longest_length{std::transform_reduce(
                                  std::begin(m_items), std::end(m_items), std::size_t{},
                                  [](const auto a, const auto b) { return std::max(a, b); },
                                  [](const char *str) { return std::strlen(str); }) +
                              4U};

    /* based on the longest string, we need to add a buffer around it */
    const auto width_in_chars{longest_length + 2 * g_cfg.startcol};
    const auto height_in_chars{std::size(m_items) * g_cfg.row_spacing + g_cfg.titlestartline + 2};

    const auto rect_shape{details::to_size_in_pixels({.width = width_in_chars, .height = height_in_chars})};

    return details::compute_screen_centered_rectangle(rect_shape);
}

void cursor_menu_dialog::draw_letter(const uint32_t xpos, const uint32_t ypos, const char letter) const noexcept
{
    const auto xpix{m_menu_rect.topleft.x + xpos * GRID_WIDTH};
    const auto ypix{m_menu_rect.topleft.y + ypos * GRID_HEIGHT};
    draw_standard_character_to_4bpp_display(letter, xpix, ypix, m_palette.font_color, m_palette.background);
}

void cursor_menu_dialog::draw_menu() const noexcept
{
    screen::pause_screen();

    /* draw the title */
    {
        uint32_t ypos{g_cfg.titlestartline};
        uint32_t xpos{g_cfg.startcol};
        for (uint32_t idx = 0; m_title[idx] != '\0'; ++idx)
        {
            draw_letter(xpos, ypos, m_title[idx]);
            xpos++;
        }
    }

    /* draw the options */
    uint32_t ypos{g_cfg.startline};
    for (uint32_t idx = 0; idx < std::size(m_items); ++idx)
    {
        uint32_t xpos{g_cfg.startcol};
        draw_letter(xpos, ypos, '[');
        xpos++;
        draw_letter(xpos, ypos, ' ');
        xpos++;
        draw_letter(xpos, ypos, ']');
        xpos++;
        draw_letter(xpos, ypos, ' ');
        xpos++;

        const char *c_str{m_items[idx]};
        for (uint32_t idx = 0; c_str[idx] != '\0'; ++idx)
        {
            draw_letter(xpos, ypos, c_str[idx]);
            xpos++;
        }
        ypos += g_cfg.row_spacing;
    }

    screen::resume_screen();
}

int cursor_menu_dialog::process_menu() noexcept
{

    /* initialize the menu configuration */
    bool wait_for_noaction{true};
    int result = 0;

    m_prev_cursor = std::size(m_items);
    m_cursor.reset();

    draw_menu();

    bool in_menu{true};
    while (in_menu)
    {
        /* draw the menu */
        update_selection();

        /* read user input
         * if "up" or "down", move selection cursor
         * if "etc", launch the registered program
         * otherwise, ignore
         *
         */
        const UserInstruction input{process_user_input()};
        switch (input)
        {
        case UserInstruction::CURSOR_UP:
            if (!wait_for_noaction)
            {
                m_cursor--;
                wait_for_noaction = true;
            }
            break;
        case UserInstruction::CURSOR_DOWN:
            if (!wait_for_noaction)
            {
                m_cursor++;
                wait_for_noaction = true;
            }
            break;
        case UserInstruction::LAUNCH_PROGRAM:
            if (!wait_for_noaction)
            {
                in_menu = false;
                wait_for_noaction = true;
                result = m_cursor;
            }
            break;
        case UserInstruction::NOACTION:
            wait_for_noaction = false;
            break;
        }
    }

    return result;
}

void cursor_menu_dialog::update_selection() noexcept
{
    if (m_prev_cursor != m_cursor)
    {
        screen::pause_screen();
        /* actually update the screen*/
        const uint32_t xpos{g_cfg.startcol};
        uint32_t ypos{g_cfg.startline};
        for (uint32_t idx = 0; idx < std::size(m_items); ++idx)
        {
            draw_letter(xpos, ypos, '[');
            draw_letter(xpos + 1, ypos, ' ');
            ypos += g_cfg.row_spacing;
        }

        ypos = g_cfg.startline + g_cfg.row_spacing * m_cursor;
        draw_letter(xpos, ypos, '[');
        draw_letter(xpos + 1, ypos, '*');

        m_prev_cursor = m_cursor;
        screen::resume_screen();
    }
}

} // namespace screen::gfx