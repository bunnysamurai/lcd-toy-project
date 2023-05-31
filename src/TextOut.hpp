#ifndef TEXTOUT_HPP
#define TEXTOUT_HPP

#include <cstddef>
#include <array>
#include "../glyphs/letters.hpp"

// helpers?

constexpr bool check_if_printable(const char c)
{
    // defintely not printable if bits 6 and 5 are unset, if all bit are set, or if only bit 5 is set
    //  ASCII is not arbitrarily layed out, it would seem (https://en.wikipedia.org/wiki/ASCII).
    // or, if c > 31 and c < 127
    return c > 31 && c < 127;
}
constexpr bool check_if_newline(const char c)
{
    return c == '\n';
}
constexpr bool check_if_tab(const char c)
{
    return c == '\t';
}
constexpr bool check_if_null(const char ch)
{
    return ch == '\0';
}

/**
 *
 */
template <class buffer_type>
class TextOut
{

private:
    static constexpr auto MAX_CHARACTER_COLUMN_COUNT{buffer_type::template max_tiles_per_row<glyphs::LetterType>()};
    static constexpr auto MAX_ROW_COUNT{buffer_type::template max_tiles_per_column<glyphs::LetterType>()};
    uint column;
    uint line;
    buffer_type &buffer;

    constexpr void increment_column()
    {
        const auto rv{column + 1};
        if (rv == MAX_CHARACTER_COLUMN_COUNT)
        {
            jump_to_new_row();
        }
        else
        {
            column = rv;
        }
    }
    constexpr void increment_row()
    {
        // TODO When reaching the bottom, we just jump to the top of the screen.  Really?
        line = line + 1 == MAX_ROW_COUNT ? 0 : line + 1;
    }
    constexpr void jump_to_new_row()
    {
        increment_row();
        column = 0;
    }

public:
    constexpr explicit TextOut(buffer_type &buf) : column{0}, line{0}, buffer{buf} {}

    friend constexpr void clear(TextOut &dev)
    {
        clear(dev.buffer);
        dev.column = 0;
        dev.line = 0;
    }

    template <size_t N>
    friend constexpr void
    print(TextOut &dev, const char (&str)[N])
    {
        for (size_t ii = 0; ii < N; ++ii)
        {
            if (check_if_null(str[ii]))
            {
                break;
            }
            putc(dev, str[ii]);
        }
    }

    friend constexpr void putc(TextOut &dev, char c)
    {
        if (check_if_printable(c))
        {
            write_tile(dev.buffer, glyphs::decode_ascii(c), dev.column, dev.line);
            dev.increment_column();
            return;
        }
        if (check_if_newline(c))
        {
            dev.jump_to_new_row();
            return;
        }
        if (check_if_tab(c))
        {
            write_tile(dev.buffer, glyphs::decode_ascii(' '), dev.column, dev.line);
            dev.increment_column();
            write_tile(dev.buffer, glyphs::decode_ascii(' '), dev.column, dev.line);
            dev.increment_column();
            return;
        }
        write_tile(dev.buffer, glyphs::decode_ascii(static_cast<char>(1)), dev.column, dev.line);
        dev.increment_column();
    }
};

#endif