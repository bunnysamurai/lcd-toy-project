#ifndef LETTERS_HPP
#define LETTERS_HPP

#include <array>
#include <cstdint>
#include <cstddef>
#include <optional>
#include "bit_manip.hpp"

namespace glyphs
{
    // using LetterType = std::array<uint8_t, 8>;
    struct LetterType
    {
        std::array<uint8_t, 8> m_data;

        [[nodiscard]] constexpr uint8_t &operator[](size_t ii) { return m_data[ii]; }
        [[nodiscard]] constexpr const uint8_t &operator[](size_t ii) const { return m_data[ii]; }
        [[nodiscard]] friend constexpr size_t size(const LetterType &letter)
        {
            using std::size;
            return size(letter.m_data);
        }
        [[nodiscard]] friend constexpr LetterType reverse(LetterType letter)
        {
            for (auto &c : letter.m_data)
            {
                c = bit_manip::reverse_bits(c);
            }
            return letter;
        }
        [[nodiscard]] friend constexpr LetterType invert(LetterType letter)
        {
            for (auto &c : letter.m_data)
            {
                c = ~c;
            }
            return letter;
        }
    };

    namespace details
    {
        inline constexpr LetterType Stub()
        {
            return {
                {0b000000'00,
                 0b001100'00,
                 0b010010'00,
                 0b100001'00,
                 0b000000'00,
                 0b010010'00,
                 0b000000'00}};
        }
        // Let's do 8x8 row, cols
        inline constexpr LetterType A()
        {
            return {
                {0b001100'00,
                 0b010010'00,
                 0b010010'00,
                 0b011110'00,
                 0b100001'00,
                 0b100001'00,
                 0b100001'00}};
        }
        inline constexpr LetterType B()
        {
            return {
                {0b111110'00,
                 0b100011'00,
                 0b100010'00,
                 0b111110'00,
                 0b100010'00,
                 0b100011'00,
                 0b111110'00}};
        }
        inline constexpr LetterType C()
        {
            return {
                {0b001111'00,
                 0b110000'00,
                 0b100000'00,
                 0b100000'00,
                 0b100000'00,
                 0b110000'00,
                 0b001111'00}};
        }
        inline constexpr LetterType D()
        {
            return {
                {0b111100'00,
                 0b100011'00,
                 0b100001'00,
                 0b100001'00,
                 0b100001'00,
                 0b100011'00,
                 0b111100'00}};
        }
        inline constexpr LetterType E()
        {
            return {
                {0b111111'00,
                 0b100000'00,
                 0b100000'00,
                 0b111110'00,
                 0b100000'00,
                 0b100000'00,
                 0b111111'00}};
        }
        inline constexpr LetterType F()
        {
            return {
                {0b111111'00,
                 0b100000'00,
                 0b100000'00,
                 0b111100'00,
                 0b100000'00,
                 0b100000'00,
                 0b100000'00}};
        }
        inline constexpr LetterType G()
        {
            return {
                {0b001111'00,
                 0b110000'00,
                 0b100000'00,
                 0b100110'00,
                 0b100001'00,
                 0b110001'00,
                 0b001111'00}};
        }
        inline constexpr LetterType H()
        {
            return {
                {0b100001'00,
                 0b100001'00,
                 0b100001'00,
                 0b111111'00,
                 0b100001'00,
                 0b100001'00,
                 0b100001'00}};
        }
        inline constexpr LetterType I()
        {
            return {
                {0b111111'00,
                 0b000100'00,
                 0b000100'00,
                 0b000100'00,
                 0b000100'00,
                 0b000100'00,
                 0b111111'00}};
        }
        inline constexpr LetterType J()
        {
            return {
                {0b000011'00,
                 0b000001'00,
                 0b000001'00,
                 0b000001'00,
                 0b000001'00,
                 0b000110'00,
                 0b111000'00}};
        }
        inline constexpr LetterType K()
        {
            return {
                {0b100110'00,
                 0b101100'00,
                 0b110000'00,
                 0b101000'00,
                 0b100100'00,
                 0b100010'00,
                 0b100001'00}};
        }
        inline constexpr LetterType L()
        {
            return {
                {0b100000'00,
                 0b100000'00,
                 0b100000'00,
                 0b100000'00,
                 0b100000'00,
                 0b100000'00,
                 0b111111'00}};
        }
        inline constexpr LetterType M()
        {
            return {
                {0b100001'00,
                 0b110011'00,
                 0b101101'00,
                 0b101001'00,
                 0b100001'00,
                 0b100001'00,
                 0b100001'00}};
        }
        inline constexpr LetterType N()
        {
            return {
                {0b100001'00,
                 0b110001'00,
                 0b101001'00,
                 0b101001'00,
                 0b100101'00,
                 0b100011'00,
                 0b100001'00}};
        }
        inline constexpr LetterType O()
        {
            return {
                {0b011110'00,
                 0b100001'00,
                 0b100001'00,
                 0b100001'00,
                 0b100001'00,
                 0b100001'00,
                 0b011110'00}};
        }
        inline constexpr LetterType P()
        {
            return {
                {0b011110'00,
                 0b100001'00,
                 0b100001'00,
                 0b111110'00,
                 0b100000'00,
                 0b100000'00,
                 0b100000'00}};
        }
        inline constexpr LetterType Q()
        {
            return {
                {0b011110'00,
                 0b100001'00,
                 0b100001'00,
                 0b100001'00,
                 0b100101'00,
                 0b111111'00,
                 0b000001'00}};
        }
        inline constexpr LetterType R()
        {
            return {
                {0b011110'00,
                 0b100001'00,
                 0b100001'00,
                 0b111110'00,
                 0b100110'00,
                 0b100010'00,
                 0b100001'00}};
        }
        inline constexpr LetterType S()
        {
            return {
                {0b001110'00,
                 0b010001'00,
                 0b100000'00,
                 0b011110'00,
                 0b000001'00,
                 0b100001'00,
                 0b011110'00}};
        }
        inline constexpr LetterType T()
        {
            return {
                {0b111111'00,
                 0b000100'00,
                 0b000100'00,
                 0b000100'00,
                 0b000100'00,
                 0b000100'00,
                 0b000100'00}};
        }
        inline constexpr LetterType U()
        {
            return {
                {0b100001'00,
                 0b100001'00,
                 0b100001'00,
                 0b100001'00,
                 0b100001'00,
                 0b100001'00,
                 0b011110'00}};
        }
        inline constexpr LetterType V()
        {
            return {
                {0b100001'00,
                 0b100001'00,
                 0b100001'00,
                 0b010010'00,
                 0b010010'00,
                 0b010010'00,
                 0b001100'00}};
        }
        inline constexpr LetterType W()
        {
            return {
                {0b100001'00,
                 0b100001'00,
                 0b100001'00,
                 0b101101'00,
                 0b101101'00,
                 0b011110'00,
                 0b010010'00}};
        }
        inline constexpr LetterType X()
        {
            return {
                {0b100001'00,
                 0b100001'00,
                 0b010010'00,
                 0b001100'00,
                 0b010010'00,
                 0b100001'00,
                 0b100001'00}};
        }
        inline constexpr LetterType Y()
        {
            return {
                {0b100001'00,
                 0b010010'00,
                 0b001100'00,
                 0b001000'00,
                 0b001000'00,
                 0b001000'00,
                 0b001000'00}};
        }
        inline constexpr LetterType Z()
        {
            return {
                {0b111111'00,
                 0b000011'00,
                 0b000010'00,
                 0b000100'00,
                 0b001000'00,
                 0b010000'00,
                 0b111111'00}};
        }

        // non alpha-numeric
        inline constexpr LetterType Space()
        {
            return {};
        }

        // numbers
        inline constexpr LetterType Number_Zero()
        {
            return {
                {0b011110'00,
                 0b100001'00,
                 0b110001'00,
                 0b101101'00,
                 0b100011'00,
                 0b100001'00,
                 0b011110'00}};
        }
        inline constexpr LetterType Number_One()
        {
            return {
                {0b001100'00,
                 0b010100'00,
                 0b100100'00,
                 0b000100'00,
                 0b000100'00,
                 0b000100'00,
                 0b111111'00}};
        }
        inline constexpr LetterType Number_Two()
        {
            return {
                {0b011000'00,
                 0b100100'00,
                 0b000010'00,
                 0b001110'00,
                 0b010000'00,
                 0b100000'00,
                 0b111111'00}};
        }
        inline constexpr LetterType Number_Three()
        {
            return {
                {0b011100'00,
                 0b100010'00,
                 0b000001'00,
                 0b001111'00,
                 0b000001'00,
                 0b100010'00,
                 0b011100'00}};
        }
        inline constexpr LetterType Number_Four()
        {
            return {
                {0b000100'00,
                 0b001100'00,
                 0b010100'00,
                 0b111111'00,
                 0b000100'00,
                 0b000100'00,
                 0b000100'00}};
        }
        inline constexpr LetterType Number_Five()
        {
            return {
                {0b111111'00,
                 0b100000'00,
                 0b100000'00,
                 0b111110'00,
                 0b000011'00,
                 0b000001'00,
                 0b111110'00}};
        }
        inline constexpr LetterType Number_Six()
        {
            return {
                {0b011111'00,
                 0b100000'00,
                 0b100000'00,
                 0b011110'00,
                 0b100001'00,
                 0b100001'00,
                 0b011110'00}};
        }
        inline constexpr LetterType Number_Seven()
        {
            return {
                {0b111111'00,
                 0b000011'00,
                 0b000010'00,
                 0b000100'00,
                 0b001000'00,
                 0b010000'00,
                 0b100000'00}};
        }
        inline constexpr LetterType Number_Eight()
        {
            return {
                {0b011110'00,
                 0b100001'00,
                 0b100001'00,
                 0b011110'00,
                 0b100001'00,
                 0b100001'00,
                 0b011110'00}};
        }
        inline constexpr LetterType Number_Nine()
        {
            return {
                {0b011110'00,
                 0b100001'00,
                 0b100001'00,
                 0b011111'00,
                 0b000001'00,
                 0b000001'00,
                 0b000001'00}};
        }
    }

    /** I'll think about this more, but this type will probably be abandonded. */
    struct AsciiType
    {
        enum struct Flag_t : uint8_t
        {
            PRINTABLE,
            NEWLINE,
            TAB,
            OTHER
        } m_flags;
        std::optional<LetterType> m_letter;

        friend constexpr void write_letter(auto &buf, const AsciiType &type, uint32_t x, uint32_t y)
        {
            switch (type.m_flags)
            {
            case AsciiType::Flag_t::PRINTABLE:
                write_letter(buf, type.m_letter, x, y);
                break;
            case AsciiType::Flag_t::NEWLINE:
            case AsciiType::Flag_t::TAB:
            case AsciiType::Flag_t::OTHER:
                break;
            }
        }
    };

    constexpr auto init_letter_list()
    {
        // TODO STUB
        constexpr auto num{[](const char ch)
                           {
                               return static_cast<size_t>(ch) - 32;
                           }};
        static_assert(num(' ') == 0, "charcter SP is not at 32?");
        std::array<LetterType, 6 * 16 - 1> rv{};
        for (auto &c : rv)
        {
            c = reverse(invert(details::Stub()));
        }

        rv[num(' ')] = reverse(invert(details::Space()));

        rv[num('0')] = reverse(invert(details::Number_Zero()));
        rv[num('1')] = reverse(invert(details::Number_One()));
        rv[num('2')] = reverse(invert(details::Number_Two()));
        rv[num('3')] = reverse(invert(details::Number_Three()));
        rv[num('4')] = reverse(invert(details::Number_Four()));
        rv[num('5')] = reverse(invert(details::Number_Five()));
        rv[num('6')] = reverse(invert(details::Number_Six()));
        rv[num('7')] = reverse(invert(details::Number_Seven()));
        rv[num('8')] = reverse(invert(details::Number_Eight()));
        rv[num('9')] = reverse(invert(details::Number_Nine()));

        rv[num('A')] = reverse(invert(details::A()));
        rv[num('B')] = reverse(invert(details::B()));
        rv[num('C')] = reverse(invert(details::C()));
        rv[num('D')] = reverse(invert(details::D()));
        rv[num('E')] = reverse(invert(details::E()));
        rv[num('F')] = reverse(invert(details::F()));
        rv[num('G')] = reverse(invert(details::G()));
        rv[num('H')] = reverse(invert(details::H()));
        rv[num('I')] = reverse(invert(details::I()));
        rv[num('J')] = reverse(invert(details::J()));
        rv[num('K')] = reverse(invert(details::K()));
        rv[num('L')] = reverse(invert(details::L()));
        rv[num('M')] = reverse(invert(details::M()));
        rv[num('N')] = reverse(invert(details::N()));
        rv[num('O')] = reverse(invert(details::O()));
        rv[num('P')] = reverse(invert(details::P()));
        rv[num('Q')] = reverse(invert(details::Q()));
        rv[num('R')] = reverse(invert(details::R()));
        rv[num('S')] = reverse(invert(details::S()));
        rv[num('T')] = reverse(invert(details::T()));
        rv[num('U')] = reverse(invert(details::U()));
        rv[num('V')] = reverse(invert(details::V()));
        rv[num('W')] = reverse(invert(details::W()));
        rv[num('X')] = reverse(invert(details::X()));
        rv[num('Y')] = reverse(invert(details::Y()));
        rv[num('Z')] = reverse(invert(details::Z()));

        return rv;
    }
    /**
     * @brief I want flexibility to specify where LetterTypes live in memory.  Hence, this function to abstract that away.
     * @param c Ascii encoding, please
     * @return Reference to the equivalent LetterType, defaulting to something if unprintable (i.e. command code).
     * Behaviour undefined if char is not printable
     */
    inline const LetterType &decode_ascii(char c)
    {
        // alpha numeric for now
        // for the magic number (or expression, such as it is), see the table at the top of:
        //      https://en.wikipedia.org/wiki/ASCII
        static constexpr auto letter_list{init_letter_list()};
        static constexpr auto decode{
            [](const char ch)
            { return ch - 32; }};
        return letter_list[decode(c)];
    }
}

#endif