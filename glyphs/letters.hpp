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
    }

    inline constexpr LetterType CAPITAL_A{reverse(invert(details::A()))};
    // inline constexpr LetterType CAPITAL_B{reverse(invert(details::B()))};
    // inline constexpr LetterType CAPITAL_C{reverse(invert(details::C()))};
    // inline constexpr LetterType CAPITAL_D{reverse(invert(details::D()))};
    inline constexpr LetterType CAPITAL_E{reverse(invert(details::E()))};
    // inline constexpr LetterType CAPITAL_F{reverse(invert(details::F()))};
    // inline constexpr LetterType CAPITAL_G{reverse(invert(details::G()))};
    // inline constexpr LetterType CAPITAL_H{reverse(invert(details::H()))};
    // inline constexpr LetterType CAPITAL_I{reverse(invert(details::I()))};
    // inline constexpr LetterType CAPITAL_J{reverse(invert(details::J()))};
    // inline constexpr LetterType CAPITAL_K{reverse(invert(details::K()))};
    // inline constexpr LetterType CAPITAL_L{reverse(invert(details::L()))};
    inline constexpr LetterType CAPITAL_M{reverse(invert(details::M()))};
    inline constexpr LetterType CAPITAL_N{reverse(invert(details::N()))};
    // inline constexpr LetterType CAPITAL_O{reverse(invert(details::O()))};
    // inline constexpr LetterType CAPITAL_P{reverse(invert(details::P()))};
    // inline constexpr LetterType CAPITAL_Q{reverse(invert(details::Q()))};
    // inline constexpr LetterType CAPITAL_R{reverse(invert(details::R()))};
    inline constexpr LetterType CAPITAL_S{reverse(invert(details::S()))};
    inline constexpr LetterType CAPITAL_T{reverse(invert(details::T()))};
    // inline constexpr LetterType CAPITAL_U{reverse(invert(details::U()))};
    inline constexpr LetterType CAPITAL_V{reverse(invert(details::V()))};
    // inline constexpr LetterType CAPITAL_W{reverse(invert(details::W()))};
    // inline constexpr LetterType CAPITAL_X{reverse(invert(details::X()))};
    inline constexpr LetterType CAPITAL_Y{reverse(invert(details::Y()))};
    // inline constexpr LetterType CAPITAL_Z{reverse(invert(details::Z()))};

    inline constexpr LetterType SPACE{reverse(invert(details::Space()))};

    inline constexpr LetterType STUB{reverse(invert(details::Stub()))};

    inline constexpr LetterType NUM_0{reverse(invert(details::Number_Zero()))};
    inline constexpr LetterType NUM_1{reverse(invert(details::Number_One()))};
    inline constexpr LetterType NUM_2{reverse(invert(details::Number_Two()))};
    inline constexpr LetterType NUM_3{reverse(invert(details::Number_Three()))};
    inline constexpr LetterType NUM_4{reverse(invert(details::Number_Four()))};
    inline constexpr LetterType NUM_5{reverse(invert(details::Number_Five()))};
    inline constexpr LetterType NUM_6{reverse(invert(details::Number_Six()))};

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
            c = glyphs::STUB;
        }
        rv[num(' ')] = glyphs::SPACE;

        rv[num('0')] = glyphs::NUM_0;
        rv[num('1')] = glyphs::NUM_1;
        rv[num('2')] = glyphs::NUM_2;
        rv[num('3')] = glyphs::NUM_3;
        rv[num('4')] = glyphs::NUM_4;
        rv[num('5')] = glyphs::NUM_5;
        rv[num('6')] = glyphs::NUM_6;

        rv[num('A')] = glyphs::CAPITAL_A;
        rv[num('S')] = glyphs::CAPITAL_S;
        rv[num('T')] = glyphs::CAPITAL_T;
        rv[num('E')] = glyphs::CAPITAL_E;
        rv[num('V')] = glyphs::CAPITAL_V;
        rv[num('N')] = glyphs::CAPITAL_N;
        rv[num('M')] = glyphs::CAPITAL_M;
        rv[num('Y')] = glyphs::CAPITAL_Y;
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