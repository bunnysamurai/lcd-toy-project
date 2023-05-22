#ifndef PRINT_HPP
#define PRINT_HPP

#include <cstdint>
#include <cstddef>

#include "letters.hpp"

namespace glyphs
{

    /**
     * @param video_buf Whatever the video buffer data structure is
     * @param x Column, in characters
     * @param y Row, in characters
     * @param letter The letter to print
     */
    constexpr void write_letter(auto &video_buf, const glyphs::LetterType &letter, uint32_t x, uint32_t y)
    {
        // TODO I can see this being a customization point, with whatever type "video_buf" is having the real implementation.
        // characters are 8x8 pixels in size
        // and we need to index by character
        // x positions are byte indexes in the video buffer
        // y positions are 8row increments
        // FIXME Global macros are the worst.  Let's abstract with a type somehow.
        constexpr auto LCD_EFFECTIVE_WIDTH{DISP_WIDTH / 8};
        for (uint idx = y * LCD_EFFECTIVE_WIDTH + x, ii = 0; ii < size(letter); idx += LCD_EFFECTIVE_WIDTH, ++ii)
        {
            video_buf[idx] = letter[ii];
        }
    }

    /**
     * @param x Column, in characters
     * @param y Row, in characters
     */
    constexpr void write_tab(auto &video_buf, uint32_t x, uint32_t y)
    {
        constexpr size_t TAB_SPACE_LENGTH{2}; // TODO this needs to be configurable, maybe even at runtime?
        for (size_t ii = 0; ii < TAB_SPACE_LENGTH; ++ii)
        {
            write_letter(video_buf, glyphs::SPACE, x++, y);
        }
    }

    template <size_t N>
    constexpr void raw_print(auto &buffer, const char (&str)[N], uint32_t x, uint32_t y)
    {
        constexpr auto check_if_printable{[](const char c)
                                          {
                                              // defintely not printable if bits 6 and 5 are unset, if all bit are set, or if only bit 5 is set
                                              //  ASCII is not arbitrarily layed out, it would seem (https://en.wikipedia.org/wiki/ASCII).
                                              // or, if c > 32 and c < 127
                                              return c > 32 && c < 127;
                                          }};
        constexpr auto check_if_newline{[](const char c)
                                        {
                                            return c == '\n';
                                        }};
        constexpr auto check_if_tab{[](const char c)
                                    { return c == '\t'; }};
        constexpr auto check_if_null{[](const char ch)
                                     { return ch == '\0'; }};
        const auto x_start{x};
        for (size_t ii = 0; ii < N; ++ii)
        {
            const auto c{str[ii]};
            if (check_if_printable(c))
            {
                write_letter(buffer, glyphs::decode_ascii(str[ii]), x++, y);
                continue;
            }
            if (check_if_newline(c))
            {
                y += 8; // TODO this is very fragile...
                x = x_start;
                continue;
            }
            if (check_if_tab(c))
            {
                write_letter(buffer, glyphs::SPACE, x++, y);
                write_letter(buffer, glyphs::SPACE, x++, y);
                continue;
            }
            if (check_if_null(c))
            {
                break;
            }
            write_letter(buffer, glyphs::STUB, x++, y);
        }
    }
}

#endif