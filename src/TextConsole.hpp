#ifndef TextConsole_HPP
#define TextConsole_HPP

#include <cstddef>
#include <array>

// helpers?

[[nodiscard]]constexpr bool check_if_printable(const char c) noexcept
{
    // defintely not printable if bits 6 and 5 are unset, if all bit are set, or if only bit 5 is set
    //  ASCII is not arbitrarily layed out, it would seem (https://en.wikipedia.org/wiki/ASCII).
    // or, if c > 31 and c < 127
    return c > 31 && c < 127;
}
[[nodiscard]] constexpr bool check_if_newline(const char c) noexcept
{
    return c == '\n';
}
[[nodiscard]] constexpr bool check_if_tab(const char c) noexcept
{
    return c == '\t';
}
[[nodiscard]] constexpr bool check_if_null(const char ch) noexcept
{
    return ch == '\0';
}
[[nodiscard]] constexpr bool check_if_backspace(const char ch) noexcept
{
    return ch == 0x08;
}

/**
 *
 */
template <class tiled_screen_device>
class TextConsole
{
private:
    static constexpr auto MAX_CHARACTER_COLUMN_COUNT{tiled_screen_device::template max_tiles_per_row()};
    static constexpr auto MAX_ROW_COUNT{tiled_screen_device::template max_tiles_per_column()};
    static constexpr uint START_COLUMN{0};
    static constexpr uint START_LINE{0};
    uint column;
    uint line;
    tiled_screen_device &buffer;

    constexpr void decrement_column()
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
    constexpr void increment_column()
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
    constexpr void decrement_row()
    {
        if (line > 0)
        {
            --line;
        }
    }
    constexpr void increment_row()
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
    constexpr void jump_to_previous_row()
    {
        decrement_row();
        column = MAX_CHARACTER_COLUMN_COUNT - 1;
    }
    constexpr void jump_to_new_row()
    {
        increment_row();
        column = 0;
    }

public:
    constexpr explicit TextConsole(tiled_screen_device &buf) : column{START_COLUMN}, line{START_LINE},
                                                         buffer{buf} {}

    friend constexpr void clear(TextConsole &dev)
    {
        clear(dev.buffer);
        dev.column = START_COLUMN;
        dev.line = START_LINE;
    }

    // FIXME this is an awful function
    friend constexpr void
    print(TextConsole &dev, const char *str)
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
    print(TextConsole &dev, const char (&str)[N])
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
    print(TextConsole &dev, const char c)
    {
        if (!check_if_null(c))
        {
            putc(dev, c);
        }
    }

    friend constexpr void putc(TextConsole &dev, char c)
    {
        if (check_if_printable(c))
        {
            draw(dev.buffer, c, dev.column, dev.line);
            dev.increment_column();
            draw(dev.buffer, '_', dev.column, dev.line);
            return;
        }
        if (check_if_newline(c))
        {
            draw(dev.buffer, ' ', dev.column, dev.line);
            dev.jump_to_new_row();
            draw(dev.buffer, '_', dev.column, dev.line);
            return;
        }
        if (check_if_tab(c))
        {
            draw(dev.buffer, ' ', dev.column, dev.line);
            dev.increment_column();
            draw(dev.buffer, ' ', dev.column, dev.line);
            dev.increment_column();
            draw(dev.buffer, '_', dev.column, dev.line);
            return;
        }
        if (check_if_backspace(c))
        {
            draw(dev.buffer, ' ', dev.column, dev.line);
            dev.decrement_column();
            draw(dev.buffer, '_', dev.column, dev.line);
        }
    }
};

#endif