#include "number_dialog.hpp"

#include "../screen.hpp"
#include "details/dialog_utils.hpp"
#include "shapes.hpp"

#include "../../../src/common/screen_utils.hpp"

#include <cstring>

namespace screen::gfx
{

number_dialog::number_dialog(number_dialog_palette cfg, const char *promt, int minval, int maxval) noexcept
    : m_palette{cfg}, m_promt{prompt}, m_min{minval}, m_max{maxval}
{
}

int number_dialog::ask() noexcept
{
    /* draw the menu, which is a rectangle plus a border */
    const auto menu_rect{compute_background_shape()};

    screen::pause_screen();
    draw_rect(menu_rect, m_palette.background, 0);
    draw_border(menu_rect, 4, m_palette.bright_highlight, m_palette.shadow_highlight);
    screen::resume_screen();

    /* run the logic */
    return process_dialog();
}

int number_dialog::process_dialog() noexcept
{
    /* update the dialog string */

    /* process user input from the gamepad */

    /* if up/down, inc/dec number as appropriate */
    /* if right/left, ignore */
    /* if other/etc, return the current count */

    /* TODO we should also support other gamepad options... */
}

Rect number_dialog::compute_background_shape() noexcept
{
    /* compute max number of characters needed by the number */
    int counter{0};
    for (;;)
    {
        ++counter;
        if ((m_max / (10 * counter)) == 0)
        {
            break;
        }
    }

    /* need 2 characters for ": " and 2 characters for the buffer around the text */
    const auto width_in_chars{std::strlen(m_prompt) + 2 + 2 + counter};
    const auto height_in_chars{3};

    const auto background_shape{details::to_size_in_pixels({.width = width_in_chars, .height = height_in_chars})};

    return details::compute_screen_centered_rectangle(background_shape);
}
} // namespace screen::gfx