#include "dialog.hpp"

#include <cstdint>
#include <cstring>

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

static constexpr size_t WORD_LIMIT{64};

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
            if (column_position > limit)
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
    if (column_position > limit)
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
    bool status = true;

    /* case 1 */
    const char *test_str1{"012 456 8"};
    const auto result = determine_page_breaks(test_str1, 3);

    status &= result.size() == 2;

    /* case 2 */
    const char *test_str2{"01"};
    const auto result2 = determine_page_breaks(test_str2, 3);

    status &= result2.size() == 0;

    /* case 3 */
    const char *test_str3{"Collect chips to get past the chip socket. Use keys to open doors."};
    const auto result3 = determine_page_breaks(test_str3, 20);
    const std::array<uint8_t, 3> expectation{21, 14, 20}; /* TODO the last element fails */

    status &= result3.size() == std::size(expectation);

    // for (size_t ii = 0; ii < std::size(result3); ++ii)
    for (size_t ii = 0; ii < 3; ++ii)
    {
        status &= expectation[ii] == result3[ii];
    }

    return status;
}

#ifdef RUN_CONSTEXPR_TESTS_IN_STATIC_ASSERTS
static_assert(test_determine_page_breaks());
#endif

void init_dialog(Rect text_area) noexcept
{
#ifdef PRINTF_DEBUGGING
    printf("init_dialog text area: { x: %d, y: %d, w: %d, h: %d }\n", text_area.topleft.x, text_area.topleft.y,
           text_area.size.width, text_area.size.height);
#endif

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
    while (begin < end)
    {
    screen:
        draw_standard_character_to_4bpp_display(*begin++, topleft.x, topleft.y, g_palette.font_color,
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

void display_dialog_box(const char *str, uint32_t column_limit, Point topleft) noexcept
{
    /*
        going word by word, keep track of which column this word will end on
        if the ending column is beyond the column limit, begin the word on the next line.
     */
    const auto page_breaks{determine_page_breaks(str, column_limit)};

    if (page_breaks.empty())
    {
#ifdef PRINTF_DEBUGGING
        printf("page breaks is empty!\n");
#endif
        init_dialog(screen::gfx::Rect{
            .topleft = topleft,
            .size = {.width = column_limit * GLYPH_WIDTH, .height = GLYPH_HEIGHT},
        });
        write_substring_to_dialog(topleft, str, std::next(str, std::strlen(str)));
        return;
    }

#ifdef PRINTF_DEBUGGING
    printf("number of page breaks is %u\n", std::size(page_breaks));
#endif

    init_dialog(screen::gfx::Rect{
        .topleft = topleft,
        .size = {.width = column_limit * GLYPH_WIDTH, .height = std::size(page_breaks) * GLYPH_HEIGHT},
    });

#ifdef PRINTF_DEBUGGING
    printf("write substring for first page\n");
#endif

    /* page breaks are encoded with the first element being the start, and subsequent elements being offsets from
     * the previous. */
    uint32_t page_break_idx{page_breaks[0]};
    write_substring_to_dialog(topleft, str, std::next(str, page_break_idx));
    topleft.y += glyphs::tile::height();

    for (size_t ii = 1; ii < std::size(page_breaks); ++ii)
    {
        const uint32_t prev = page_break_idx;
        page_break_idx += page_breaks[ii];
#ifdef PRINTF_DEBUGGING
        printf("write substring for %u page\n", ii + 1);
#endif
        write_substring_to_dialog(topleft, std::next(str, prev), std::next(str, page_break_idx));
        topleft.y += glyphs::tile::height();
    }

#ifdef PRINTF_DEBUGGING
    printf("write remaining substring, if any\n");
#endif
    // write_substring_to_dialog(topleft, std::next(str, page_break_idx), std::next(str, std::strlen(str)));
#ifdef PRINTF_DEBUGGING
    printf("done\n");
#endif
}

void display_dialog_box(const char *str, uint32_t col_limit) noexcept
{
    const auto [width, height]{screen::get_virtual_screen_size()};
    const auto dialog_width{(col_limit + 2) * GLYPH_WIDTH};
    const auto dialog_y_offset{height / 3};
    const auto dialog_x_offset{(width - dialog_width) / 2};
    screen::pause_screen();
    display_dialog_box(str, col_limit, {.x = dialog_x_offset, .y = dialog_y_offset});
    screen::resume_screen();
}

} // namespace screen::gfx
