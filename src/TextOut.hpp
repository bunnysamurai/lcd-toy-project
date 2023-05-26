#ifndef TEXTOUT_HPP
#define TEXTOUT_HPP

#include <cstddef>
#include <array>

// helpers?
template <class CharType>
constexpr size_t to_character_width(const size_t screen_width_in_pixels, const size_t bits_per_pixel)
{
    constexpr auto elem_width{CharType::elem_width};
    const auto element_per_pixel{1};
    return screen_width_in_pixels * bits_per_pixel / elem_width;
}
template <class CharType>
constexpr size_t to_character_height(const size_t screen_height_in_pixels, const size_t bits_per_pixel)
{
    constexpr auto elem_height{CharType::elem_height};
    const auto element_per_pixel{1};
    return screen_height_in_pixels * bits_per_pixel / elem_height;
}
constexpr size_t compute_video_buffer_length(size_t width, size_t height, size_t bits_per_pixel)
{
    return width * height * bits_per_pixel / 8;
}

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
template <size_t SCREEN_WIDTH, size_t SCREEN_HEIGHT, size_t BPP, class LetterT>
class TextOut
{
public:
    using buffer_type = std::array<uint8_t, compute_video_buffer_length(SCREEN_WIDTH, SCREEN_HEIGHT, BPP)>;

private:
    static constexpr auto MAX_CHARACTER_COLUMN_COUNT{to_character_width<LetterT>(SCREEN_WIDTH, BPP)};
    static constexpr auto MAX_ROW_COUNT{to_character_height<LetterT>(SCREEN_HEIGHT, BPP)};
    static constexpr auto LETTER_WIDTH{LetterT::elem_width};
    static constexpr auto LETTER_HEIGHT{LetterT::elem_height};
    uint column;
    uint line;
    buffer_type &buffer;

    void increment_column()
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
    void increment_row()
    {
        line = line + 1 == MAX_ROW_COUNT ? 0 : line + 1;
    }
    void jump_to_new_row()
    {
        increment_row();
        column = 0;
    }

    /**
     * @param video_buf Whatever the video buffer data structure is
     * @param x Column, in characters
     * @param y Row, in characters
     * @param letter The letter to print
     */
    constexpr void write_letter(auto &video_buf, const LetterT &letter, uint32_t x, uint32_t y)
    {
        // TODO I can see this being a customization point, with whatever type "video_buf" is having the real implementation.
        // characters are 8x8 pixels in size
        // and we need to index by character
        // x positions are byte indexes in the video buffer
        // y positions are 8row increments
        // FIXME Global macros are the worst.  Let's abstract with a type somehow.
        for (uint idx = y * LETTER_HEIGHT * MAX_CHARACTER_COLUMN_COUNT + x, ii = 0; ii < size(letter); idx += MAX_CHARACTER_COLUMN_COUNT, ++ii)
        {
            video_buf[idx] = letter[ii];
        }
    }

public:
    constexpr explicit TextOut(buffer_type &buf) : buffer{buf} {}

    template <size_t N>
    friend constexpr void
    print(TextOut &dev, const char (&str)[N])
    {
        for (size_t ii = 0;; ++ii)
        {
            if (check_if_null(str[ii]))
            {
                break;
            }
            putc(str[ii], dev);
        }
    }

    friend constexpr void putc(char c, TextOut &dev)
    {
        if (check_if_printable(c))
        {
            dev.write_letter(dev.buffer, glyphs::decode_ascii(c), dev.column, dev.line);
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
            dev.write_letter(dev.buffer, glyphs::decode_ascii(' '), dev.column, dev.line);
            dev.increment_column();
            dev.write_letter(dev.buffer, glyphs::decode_ascii(' '), dev.column, dev.line);
            dev.increment_column();
            return;
        }
        dev.write_letter(dev.buffer, glyphs::decode_ascii(static_cast<char>(1)), dev.column, dev.line);
        dev.increment_column();
    }
};

#endif