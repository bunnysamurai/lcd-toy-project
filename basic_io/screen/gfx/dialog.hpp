#ifndef SCREEN_GFX_DIALOG_HPP
#define SCREEN_GFX_DIALOG_HPP

#include <cstdint>

#include "defs.hpp"

namespace screen::gfx
{

struct dialog_palette
{
    uint16_t bright_highlight;
    uint16_t shadow_highlight;
    uint16_t background;
    uint16_t font_color;
};

[[nodiscard]] dialog_palette get_dialog_palette() noexcept;
void set_dialog_palette(dialog_palette palette) noexcept;

/**
    @brief draw a simple dialog box on the display.

    Limited to 64 words.

    Height of the dialog box adjusts automatically to fit the text.
 */
void display_dialog_box(const char *string, uint32_t column_limit, Point topleft) noexcept;

/**
    @brief draw a simple dialog box on the display.

    Uses sane defaults for the screen size.
 */
void display_dialog_box(const char *string, uint32_t col_limit = 10) noexcept;

} // namespace screen::gfx

#endif