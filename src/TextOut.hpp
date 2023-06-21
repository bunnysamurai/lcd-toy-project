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
constexpr bool check_if_backspace(const char ch)
{
    return ch == 0x08;
}

/**
 *
 */
template <class buffer_type>
class TextOut
{
private:
    // TODO make build-time configurable, also, letters' init method cares about this, so we need to make changes in two places manually right now.
    static constexpr auto use_native_ordering{false};
    static constexpr auto use_rotated_ordering{!use_native_ordering};

    static constexpr auto MAX_CHARACTER_COLUMN_COUNT{buffer_type::template max_tiles_per_row<glyphs::LetterType>()};
    static constexpr auto MAX_ROW_COUNT{buffer_type::template max_tiles_per_column<glyphs::LetterType>()};
    static constexpr uint START_COLUMN{0};
    static constexpr uint START_LINE{[]()
                                     {
                                         if constexpr (use_native_ordering)
                                         {
                                             return 0;
                                         }
                                         else if (use_rotated_ordering)
                                         {
                                             return MAX_ROW_COUNT - 1;
                                         }
                                     }()};
    uint column;
    uint line;
    buffer_type &buffer;

    // these methods are for "native" image buffer ordering
    constexpr void decrement_column_native()
    {
        if (column == 1)
        {
            jump_to_previous_row();
        }
        else
        {
            --column;
        }
    }
    constexpr void increment_column_native()
    {
        if (column == MAX_CHARACTER_COLUMN_COUNT - 1)
        {
            jump_to_new_row();
        }
        else
        {
            ++column;
        }
    }
    constexpr void decrement_row_native()
    {
        if (line > 0)
        {
            --line;
        }
    }
    constexpr void increment_row_native()
    {
        if (line == MAX_ROW_COUNT - 1)
        {
            scroll_up(buffer, 8); // TODO magic number, it's the number of native rows to scroll, which is a function of the video buffer and the tiles used.
        }
        else
        {
            ++line;
        }
    }
    constexpr void jump_to_previous_row_native()
    {
        decrement_row();
        column = MAX_CHARACTER_COLUMN_COUNT - 1;
    }
    constexpr void jump_to_new_row_native()
    {
        increment_row();
        column = 0;
    }

    // these methods are for "rotated" image buffer ordering
    constexpr void decrement_column_rotated()
    {
        // when we decrement a "column" position, we actually increment the line position
        if (line == MAX_ROW_COUNT - 1)
        {
            jump_to_previous_row();
        }
        else
        {
            ++line;
        }
    }
    constexpr void increment_column_rotated()
    {
        // "column" in the title actually means line
        // when we increment a "column" position, we actually decrement the line position
        // if the current line position is zero, we "jump_to_new_row_rotated"
        if (line == 0)
        {
            jump_to_new_row();
        }
        else
        {
            --line;
        }
    }
    constexpr void decrement_row_rotated()
    {
        // "row" in title actually means column
        if (column > 0)
        {
            --column;
        }
    }
    constexpr void increment_row_rotated()
    {
        // "row" in title actually means column
        // column ordering is in ascending order, so we simple add one
        // if current column is one-less-the-max, reset to zero.  This effectively increments the "row"
        if (column == MAX_CHARACTER_COLUMN_COUNT - 1)
        {
            scroll_left(buffer, 1); // TODO magic number, this is the number of native columns to scrll, which is a function of the video buffer and the tiles used.
        }
        else
        {
            ++column;
        }
    }
    constexpr void jump_to_previous_row_rotated()
    {
        decrement_row();
        line = 0;
    }
    constexpr void jump_to_new_row_rotated()
    {
        // "row" in title actually means column
        // when we perform a jump, the "column" location needs to be reset
        // which, in our case, means back to max row value
        increment_row();
        line = MAX_ROW_COUNT - 1;
    }

    // FIXME SWITCHED ON MACRO.  BOO.
    constexpr void decrement_column()
    {
        if constexpr (use_native_ordering)
        {
            decrement_column_native();
        }
        else if (use_rotated_ordering)
        {
            decrement_column_rotated();
        }
    }
    constexpr void increment_column()
    {
        if constexpr (use_native_ordering)
        {
            increment_column_native();
        }
        else if (use_rotated_ordering)
        {
            increment_column_rotated();
        }
    }
    constexpr void decrement_row()
    {
        if constexpr (use_native_ordering)
        {
            decrement_row_native();
        }
        else if (use_rotated_ordering)
        {
            decrement_row_rotated();
        }
    }
    constexpr void increment_row()
    {
        if constexpr (use_native_ordering)
        {
            increment_row_native();
        }
        else if (use_rotated_ordering)
        {
            increment_row_rotated();
        }
    }
    constexpr void jump_to_previous_row()
    {
        if constexpr (use_native_ordering)
        {
            jump_to_previous_row_native();
        }
        else if (use_rotated_ordering)
        {
            jump_to_previous_row_rotated();
        }
    }
    constexpr void jump_to_new_row()
    {
        if constexpr (use_native_ordering)
        {
            jump_to_new_row_native();
        }
        else if (use_rotated_ordering)
        {
            jump_to_new_row_rotated();
        }
    }

    template <class Tile>
    constexpr Tile adjust_tile(Tile tile)
    {
        if constexpr (use_native_ordering)
        {
            return tile;
        }
        else if (use_rotated_ordering)
        {
            // return rotate(std::move(tile));
            return tile;
        }
    }

public:
    constexpr explicit TextOut(buffer_type &buf) : column{START_COLUMN}, line{START_LINE},
                                                   buffer{buf} {}

    friend constexpr void clear(TextOut &dev)
    {
        clear(dev.buffer);
        dev.column = START_COLUMN;
        dev.line = START_LINE;
    }

    // FIXME this is an awful function
    friend constexpr void
    print(TextOut &dev, const char *str)
    {
        for (size_t ii = 0; true; ++ii)
        {
            if (check_if_null(str[ii]))
            {
                break;
            }
            putc(dev, str[ii]);
        }
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
    friend constexpr void
    print(TextOut &dev, const char c)
    {
        if (!check_if_null(c))
        {
            putc(dev, c);
        }
    }

    friend constexpr void putc(TextOut &dev, char c)
    {
        if (check_if_printable(c))
        {
            draw(dev.buffer, dev.adjust_tile(glyphs::decode_ascii(c)), dev.column, dev.line);
            dev.increment_column();
            draw(dev.buffer, dev.adjust_tile(glyphs::decode_ascii('_')), dev.column, dev.line);
            return;
        }
        if (check_if_newline(c))
        {
            draw(dev.buffer, dev.adjust_tile(glyphs::decode_ascii(' ')), dev.column, dev.line);
            dev.jump_to_new_row();
            draw(dev.buffer, dev.adjust_tile(glyphs::decode_ascii('_')), dev.column, dev.line);
            return;
        }
        if (check_if_tab(c))
        {
            draw(dev.buffer, dev.adjust_tile(glyphs::decode_ascii(' ')), dev.column, dev.line);
            dev.increment_column();
            draw(dev.buffer, dev.adjust_tile(glyphs::decode_ascii(' ')), dev.column, dev.line);
            dev.increment_column();
            draw(dev.buffer, dev.adjust_tile(glyphs::decode_ascii('_')), dev.column, dev.line);
            return;
        }
        if (check_if_backspace(c))
        {
            draw(dev.buffer, dev.adjust_tile(glyphs::decode_ascii(' ')), dev.column, dev.line);
            dev.decrement_column();
            draw(dev.buffer, dev.adjust_tile(glyphs::decode_ascii('_')), dev.column, dev.line);
        }
    }
};

#endif