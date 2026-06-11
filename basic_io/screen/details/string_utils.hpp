#ifndef SCREEN_STRING_UTILS_HPP
#define SCREEN_STRING_UTILS_HPP

#include <cstdint>
#include <utility>

#include "embp/containers.hpp"
#include "string_utils_defs.hpp"

#include <iostream>

namespace screen::details
{

[[nodiscard]] constexpr bool my_is_blank(char c) noexcept
{
    return c == ' ' || c == '\t';
}

[[nodiscard]] constexpr bool my_is_newline(char c) noexcept
{
    return c == '\n';
}

template <class Iter>
[[nodiscard]] constexpr std::pair<Iter, Iter> trim_trailing_whitespace(Iter begin, Iter end) noexcept
{
    /*
        we'll be a little cheeky, and adjust the end pointer back until it hits the first non-space character, then
        increment it by one.  This is to trim any trailing whitespace.
    */
    if (!begin || end < begin)
    {
        return std::make_pair(begin, end);
    }

    --end; /* turn the end iterator into a reverse iterator... ish */

    while (begin != end)
    {
        const auto character{*end};
        if (!my_is_blank(character) && character != '\0' && !my_is_newline(character))
        {
            break;
        }
        std::advance(end, -1);
    }

    return std::make_pair(begin, std::next(end, 1));
}

/**
    @brief page break indicates where a new line should begin, which is always at the start of a new word.
*/
[[nodiscard]] constexpr embp::variable_array<uint8_t, PAGE_BREAK_WORD_LIMIT> determine_page_breaks(
    const char *str, uint32_t limit) noexcept
{
    embp::variable_array<uint8_t, PAGE_BREAK_WORD_LIMIT> result{};

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
    uint32_t beginning_of_previous_word = offset;
    while ((c = str[idx]) != '\0')
    {
        const auto column_position = idx - offset;
        if ((state_looking_for_blank && my_is_blank(c)) || my_is_newline(c))
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
                    result.push_back(beginning_of_this_word - offset);
                }
                offset = beginning_of_this_word;
            }
            state_looking_for_blank = false;
        }

        if (!state_looking_for_blank && !my_is_blank(c) && !my_is_newline(c))
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
            result.push_back(beginning_of_this_word - offset);
        }
    }

    return result;
}

#ifdef RUN_CONSTEXPR_TESTS_IN_STATIC_ASSERTS
namespace
{
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

static_assert(test_determine_page_breaks());
} // namespace
#endif

} // namespace screen::details

#endif