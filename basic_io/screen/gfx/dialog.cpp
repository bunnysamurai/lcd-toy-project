#include "dialog.hpp"

#include <cstdint>
#include <cstring>

#include "../glyphs/letters.hpp"
// #include "common/screen_utils.hpp"
#include "defs.hpp"
#include "embp/variable_array.hpp"
#include "shapes.hpp"

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

static constexpr size_t WORD_LIMIT{64};

static constexpr uint32_t GLYPH_WIDTH{glyphs::tile::width()};
static constexpr uint32_t GLYPH_HEIGHT{glyphs::tile::height()};

static constexpr uint32_t DIALOG_BORDER_THICKNESS_PIXELS{2};

/*
 ____       _            _
|  _ \ _ __(_)_   ____ _| |_ ___
| |_) | '__| \ \ / / _` | __/ _ \
|  __/| |  | |\ V / (_| | ||  __/
|_|   |_|  |_| \_/ \__,_|\__\___|

*/

namespace
{

[[nodiscard]] constexpr bool my_is_blank(char c) noexcept
{
    return c == ' ' || c == '\t';
}

/**
    @brief page break indicates where a new line should begin, which is always at the start of a new word.
*/
[[nodiscard]] constexpr embp::variable_array<uint8_t, WORD_LIMIT> determine_page_breaks(const char *str,
                                                                                        uint32_t limit) noexcept
{
    embp::variable_array<uint8_t, WORD_LIMIT> result;

    if (str == nullptr || str[0] == '\0')
    {
        return result;
    }

    uint32_t idx{0};

    /* skip any initial white space */
    char c = str[idx];
    while (my_is_blank(c))
    {
        c = str[++idx];
    }

    /* take note of the intial whitespace offset.  This will be used to help
     * keep track of the column position within a line.
     */
    uint32_t offset{idx};

    /* start looking for the next whitespace, via state machine */
    bool state_looking_for_blank = true;
    uint32_t beginning_of_this_word = offset;
    while ((c = str[idx]) != '\0')
    {
        const auto column_position = idx - offset;
        if (state_looking_for_blank && my_is_blank(c))
        {
            /* Found the end of the word.  If it is past the column limit,
             * put the page break at the start of this word.
             */
            if (column_position - 1 > limit)
            {
                /* for keeping byte useage low, we only encode the distance
                 * between page breaks, except for the first element, which is
                 * an absolute position. The implication of this is that we are
                 * limited to lines of 256 columns... should be sufficient.
                 */
                if (result.empty())
                {
                    result.push_back(beginning_of_this_word);
                }
                else
                {
                    const auto prev{result.back()};
                    result.push_back(beginning_of_this_word - prev);
                }
                offset = beginning_of_this_word;
            }
            state_looking_for_blank = false;
        }

        if (!state_looking_for_blank && !my_is_blank(c))
        {
            /* found a new word.  Make note of its starting position and start
             * looking for the first whitespace. */
            beginning_of_this_word = idx;
            state_looking_for_blank = true;
        }

        ++idx;
    }

    /* final processing needed once null terminator is reached */
    const auto column_position = idx - offset;
    if (column_position - 1 > limit)
    {
        if (result.empty())
        {
            result.push_back(beginning_of_this_word);
        }
        else
        {
            const auto prev{result.back()};
            result.push_back(beginning_of_this_word - prev);
        }
    }

    return result;
}

constexpr bool test_determine_page_breaks() noexcept
{
    const char *test_str{"012 456 8"};

    const auto result = determine_page_breaks(test_str, 3);

    bool status = true;

    status &= result.size() == 2;

    return status;
}

static_assert(test_determine_page_breaks());

void init_dialog(Rect text_area) noexcept
{

    /* background */
    screen::gfx::draw_rect(text_area, g_palette.background, 0);

    /* border */
    const auto top{text_area.y - DIALOG_BORDER_THICKNESS_PIXELS};
    const auto bot{text_area.y + text_area.height + DIALOG_BORDER_THICKNESS_PIXELS * 2};
    const auto left{text_area.x - DIALOG_BORDER_THICKNESS_PIXELS};
    const auto right{text_area.x + text_area.width + DIALOG_BORDER_THICKNESS_PIXELS * 2};

    for (uint32_t ii = 0; ii < DIALOG_BORDER_THICKNESS_PIXELS; ++ii)
    {
        /* top and left bright highlights */
        screen::gfx::draw_line(
            {
                .x = left + ii,
                .y = top + ii,
            },
            {
                .x = right - ii,
                .y = top - ii,
            },
            g_palette.bright_highlight, 1);

        screen::gfx::draw_line(
            {
                .x = left + ii,
                .y = top + ii,
            },
            {
                .x = left - ii,
                .y = bot - ii,
            },
            g_palette.bright_highlight, 1);

        /* bot and right dim highlights */

        /*right*/
        screen::gfx::draw_line(
            {
                .x = right - ii,
                .y = top + 1 + ii,
            },
            {
                .x = right - ii,
                .y = bot - ii,
            },
            g_palette.shadow_highlight, 1);

        /* bot*/
        screen::gfx::draw_line(
            {
                .x = left + 1 + ii,
                .y = bot - ii,
            },
            {
                .x = right - ii,
                .y = bot - ii,
            },
            g_palette.shadow_highlight, 1);
    }
}

void write_substring_to_dialog(Point topleft, const char *begin, const char *end) noexcept
{
    letter_4bpp_array_t letter_data;
    while (begin != end)
    {
        /* convert to 4bpp on the fly */
        screen::get_letter_data_4bpp(std::data(letter_data), *begin++, g_palette.font_color, g_palette.background);
        const screen::Tile tile{
            .side_length = GLYPH_WIDTH,
            .transparent = 0,
            .format = screen::get_format(),
            .data = std::data(letter_data),
        };
        screen::draw_tile(topleft.x, topleft.y, tile);
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

void display_dialog_box(const char *string, uint32_t column_limit, Point topleft) noexcept
{
    /*
        going word by word, keep track of which column this word will end on
        if the ending column is beyond the column limit, begin the word on the next line.
     */
    const auto page_breaks{determine_page_breaks(string, column_limit)};

    if (page_breaks.empty())
    {
        return;
    }

    init_dialog(screen::gfx::Rect{
        .topleft = topleft,
        .size = {.width = column_limit * GLYPH_WIDTH, .height = std::size(page_breaks) * GLYPH_HEIGHT},
    });

    /* page breaks are encoded with the first element being the start, and subsequent elements being offsets from
     * the previous. */
    uint32_t page_break_idx{page_breaks[0]};
    write_substring_to_dialog(topleft, string, std::next(string, page_break_idx));
    ++topleft.y;

    for (size_t ii = 1; ii < std::size(page_breaks); ++ii)
    {
        const uint32_t prev = page_break_idx;
        page_break_idx += page_breaks[ii];
        write_substring_to_dialog(topleft, std::next(string, prev), std::next(string, page_break_idx));
        ++topleft.y;
    }

    write_substring_to_dialog(topleft, std::next(string, page_break_idx), std::next(string, std::strlen(string)));
}

void display_dialog_box(const char *string) noexcept
{
    const auto [width, height]{screen::get_virtual_screen_size()};
    const auto dialog_width{(20 + 2) * GLYPH_WIDTH};
    const auto dialog_y_offset{height / 3};
    const auto dialog_x_offset{(width - dialog_width) / 2};
    display_dialog_box(string, 20, {.x = dialog_x_offset, .y = dialog_y_offset});
}

} // namespace screen::gfx
