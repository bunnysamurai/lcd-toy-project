#include "dialog.hpp"

#include <cstdint>
#include <cstring>
#include <algorithm>

#include "../details/string_utils.hpp"
#include "../glyphs/letter_utils.hpp"
#include "../glyphs/letters.hpp"
#include "defs.hpp"
#include "embp/variable_array.hpp"
#include "shapes.hpp"

// #define PRINTF_DEBUGGING
#ifdef PRINTF_DEBUGGING
#include "pico/printf.h"
#endif

namespace screen::gfx
{

/*
__     __
\ \   / /_ _ _ __ ___
 \ \ / / _` | '__/ __|
  \ V / (_| | |  \__ \
   \_/ \__,_|_|  |___/

*/

static dialog_palette g_palette{
    .bright_highlight = 0,
    .shadow_highlight = 1,
    .background = 2,
    .font_color = 3,
};

static constexpr uint32_t GLYPH_WIDTH{glyphs::tile::width()};
static constexpr uint32_t GLYPH_HEIGHT{glyphs::tile::height()};

static constexpr uint32_t DIALOG_BORDER_THICKNESS_PIXELS{4};

/*
 ____       _            _
|  _ \ _ __(_)_   ____ _| |_ ___
| |_) | '__| \ \ / / _` | __/ _ \
|  __/| |  | |\ V / (_| | ||  __/
|_|   |_|  |_| \_/ \__,_|\__\___|

*/

namespace
{

void init_dialog(Rect text_area) noexcept
{
    /* background */
    screen::gfx::draw_rect(text_area, g_palette.background, 0);

    /* border */
    const auto top{text_area.topleft.y - DIALOG_BORDER_THICKNESS_PIXELS};
    const auto bot{text_area.topleft.y + text_area.size.height + DIALOG_BORDER_THICKNESS_PIXELS * 2};
    const auto left{text_area.topleft.x - DIALOG_BORDER_THICKNESS_PIXELS};
    const auto right{text_area.topleft.x + text_area.size.width + DIALOG_BORDER_THICKNESS_PIXELS * 2};

    /*
    top left lines first,
    these will stretch to the bottom of the text area and all the way to the right for the entire dialog

    for bottom right lines, the same

    +--------------------------
    |                         |
    |                         |
    |                         |
    |                         |
    --------------------------+


    */
    screen::gfx::Point topleft{
        .x = text_area.topleft.x - DIALOG_BORDER_THICKNESS_PIXELS,
        .y = text_area.topleft.y - DIALOG_BORDER_THICKNESS_PIXELS,
    };
    for (uint32_t ii = 0; ii < DIALOG_BORDER_THICKNESS_PIXELS; ++ii)
    {
        const uint32_t xwidth{text_area.size.width + (DIALOG_BORDER_THICKNESS_PIXELS - ii) * 2};
        const uint32_t yheight{text_area.size.height + (DIALOG_BORDER_THICKNESS_PIXELS - ii) * 2};

        const screen::gfx::Point botleft{.x = topleft.x, .y = topleft.y + yheight - 1};
        const screen::gfx::Point topright{.x = topleft.x + xwidth - 1, .y = topleft.y};
        const screen::gfx::Point botright{.x = topright.x, .y = botleft.y};

        screen::gfx::draw_line(botleft, botright, g_palette.shadow_highlight, 1);
        screen::gfx::draw_line(topright, botright, g_palette.shadow_highlight, 1);

        screen::gfx::draw_line(topleft, topright, g_palette.bright_highlight, 1);
        screen::gfx::draw_line(topleft, botleft, g_palette.bright_highlight, 1);

        ++topleft.x;
        ++topleft.y;
    }
}

void write_substring_to_dialog(Point topleft, const char *begin, const char *end) noexcept
{
    letter_4bpp_array_t letter_data;

    auto [trimbeg, trimend]{details::trim_trailing_whitespace(begin, end)};

    while (trimbeg < trimend)
    {

        screen::draw_standard_character_to_4bpp_display(*trimbeg++, topleft.x, topleft.y, g_palette.font_color,
                                                        g_palette.background);
        topleft.x += GLYPH_WIDTH;
    }
}

} // namespace

/*
 ____        _     _ _
|  _ \ _   _| |__ | (_) ___
| |_) | | | | '_ \| | |/ __|
|  __/| |_| | |_) | | | (__
|_|    \__,_|_.__/|_|_|\___|

*/

dialog_palette get_dialog_palette() noexcept
{
    return g_palette;
}

void set_dialog_palette(dialog_palette palette) noexcept
{
    g_palette = palette;
}

void display_dialog_box(const char *str, uint32_t column_limit, Point centerpoint) noexcept
{
    /*
        going word by word, keep track of which column this word will end on
        if the ending column is beyond the column limit, begin the word on the next line.
     */
    const auto page_breaks{details::determine_page_breaks(str, column_limit)};

    const auto maxchars { *std::max_element(std::begin(page_breaks), std::end(page_breaks))};

    const auto dwidth{maxchars < column_limit ? maxchars * GLYPH_WIDTH : column_limit * GLYPH_WIDTH};
    const auto dheight{(1 + std::size(page_breaks)) * GLYPH_HEIGHT};
    const auto dialog_rect = screen::gfx::Rect{
        .topleft = {.x = centerpoint.x - dwidth / 2, .y = centerpoint.y - dheight / 2},
        .size = {.width = dwidth, .height = dheight},
    };

    init_dialog(dialog_rect);

    /* handle simple, one line case and return */
    if (page_breaks.empty())
    {
        write_substring_to_dialog(dialog_rect.topleft, str, std::next(str, std::strlen(str)));
        return;
    }

    /* page breaks are encoded with the first element being the start, and subsequent elements being offsets from
     * the previous. */
    uint32_t page_break_idx{page_breaks[0]};
    auto topleft{dialog_rect.topleft};
    write_substring_to_dialog(topleft, str, std::next(str, page_break_idx));
    topleft.y += glyphs::tile::height();

    for (size_t ii = 1; ii < std::size(page_breaks); ++ii)
    {
        const uint32_t prev = page_break_idx;
        page_break_idx += page_breaks[ii];
        write_substring_to_dialog(topleft, std::next(str, prev), std::next(str, page_break_idx));
        topleft.y += glyphs::tile::height();
    }

    write_substring_to_dialog(topleft, std::next(str, page_break_idx), std::next(str, std::strlen(str)));
}

void display_dialog_box(const char *str, uint32_t col_limit) noexcept
{
    const auto [width, height]{screen::get_virtual_screen_size()};
    const auto dialog_width{(col_limit + 2) * GLYPH_WIDTH};
    const auto dialog_y_offset{17 * height / 32};
    const auto dialog_x_offset{width / 2};
    screen::pause_screen();
    display_dialog_box(str, col_limit, {.x = dialog_x_offset, .y = dialog_y_offset});
    screen::resume_screen();
}

} // namespace screen::gfx
