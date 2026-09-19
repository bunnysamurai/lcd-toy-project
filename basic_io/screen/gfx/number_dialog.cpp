#include "number_dialog.hpp"

#include "../../../src/common/screen_utils.hpp"
#include "../../gamepad/gamepad.hpp"
#include "../glyphs/letter_utils.hpp"
#include "../glyphs/letters.hpp"
#include "../screen.hpp"
#include "details/dialog_utils.hpp"
#include "embp/constexpr_numeric.hpp"
#include "shapes.hpp"

#include "pico/time.h"

#include <cstdint>
#include <cstring>

namespace screen::gfx
{

/*===============================================================================*/
/*===============================================================================*/
namespace
{

struct ScreenLockUnlock
{
    ScreenLockUnlock() noexcept
    {
        screen::pause_screen();
    }

    ~ScreenLockUnlock() noexcept
    {
        screen::resume_screen();
    }

    /* disable all other special member functions */
    ScreenLockUnlock(const ScreenLockUnlock &) noexcept = delete;
    ScreenLockUnlock(ScreenLockUnlock &&) noexcept = delete;
    ScreenLockUnlock &operator=(const ScreenLockUnlock &) noexcept = delete;
    ScreenLockUnlock &operator=(ScreenLockUnlock &&) noexcept = delete;
};

enum struct UserInstruction
{
    NOACTION,
    INCREMENT_NUMBER,
    DECREMENT_NUMBER,
    EXIT
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

[[nodiscard]] UserInstruction read_user_input() noexcept
{
    UserInstruction result{UserInstruction::NOACTION};
    auto state{gamepad::five::get()};

    if (state.up)
    {
        result = UserInstruction::INCREMENT_NUMBER;
    }
    if (state.down)
    {
        result = UserInstruction::DECREMENT_NUMBER;
    }
    if (state.etc)
    {
        result = UserInstruction::EXIT;
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

/*===============================================================================*/
/*===============================================================================*/
number_dialog::number_dialog(number_dialog_palette cfg, const char *prompt, int minval, int maxval) noexcept
    : m_palette{cfg}, m_prompt{prompt}, m_min{minval}, m_max{maxval}, m_menu_rect{compute_background_shape()}
{
    init_menu_cfg();
}

void number_dialog::draw_dialog_background() const noexcept
{
    const ScreenLockUnlock lck{};

    draw_rect(m_menu_rect, m_palette.background, 0);
    draw_border(m_menu_rect, 4, m_palette.bright_highlight, m_palette.shadow_highlight);
}

void number_dialog::draw_dialog_promt() const noexcept
{
    const ScreenLockUnlock lck{};

    /* actually update the screen*/
    const uint32_t xpos{g_cfg.startcol};
    uint32_t ypos{g_cfg.startline};

    printf("m_prompt length is %d\n", std::strlen(m_prompt));
    printf("m_prompt: %s\n", m_prompt);
    for (uint32_t idx = 0; idx < std::strlen(m_prompt); ++idx)
    {
        draw_letter(xpos + idx, ypos, *(m_prompt+idx));
    }
}

int number_dialog::ask() noexcept
{
    /* run the logic */
    return process_dialog();
}

void number_dialog::draw_dialog_number(int number) const noexcept
{
    const ScreenLockUnlock lck{};

    /* actually update the screen*/
    uint32_t xpos{g_cfg.startcol};
    uint32_t ypos{g_cfg.startline + 1};
    const auto bcd_array{screen::bcd<3>(number)};

    for (const auto val : bcd_array)
    {
        draw_letter(xpos++, ypos, static_cast<char>('0' + val));
    }
}

int number_dialog::process_dialog() noexcept
{
    /* update the dialog string */
    draw_dialog_background();
    draw_dialog_promt();

    /* process user input from the gamepad */
    int number{m_min};
    for (;;)
    {
        draw_dialog_number(number);

        switch (read_user_input())
        {
        case UserInstruction::NOACTION:
            break;
        case UserInstruction::INCREMENT_NUMBER:
            embp::adjust_with_clamp(number, 1, m_min, m_max);
            break;
        case UserInstruction::DECREMENT_NUMBER:
            embp::adjust_with_clamp(number, -1, m_min, m_max);
            break;
        case UserInstruction::EXIT:
            return number;
        }
        sleep_ms(100);
    }
}

Rect number_dialog::compute_background_shape() const noexcept
{
    const auto width_in_chars{std::strlen(m_prompt) + 2 * g_cfg.startcol};
    const auto height_in_chars{4};

    const auto rect_shape{details::to_size_in_pixels({.width = width_in_chars, .height = height_in_chars})};

    return details::compute_screen_centered_rectangle(rect_shape);
}

void number_dialog::draw_letter(const uint32_t xpos, const uint32_t ypos, const char letter) const noexcept
{
    const auto xpix{m_menu_rect.topleft.x + xpos * GRID_WIDTH};
    const auto ypix{m_menu_rect.topleft.y + ypos * GRID_HEIGHT};
    draw_standard_character_to_4bpp_display(letter, xpix, ypix, m_palette.font_color, m_palette.background);
}

} // namespace screen::gfx