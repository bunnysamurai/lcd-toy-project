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
        static constexpr size_t elem_width{8};  // TODO in bits?
        static constexpr size_t elem_height{8}; // in bits?
        std::array<uint8_t, elem_height> m_data;

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
        // Let's do 8x8 row, cols

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

        /* ========================================================================= */
        /* ========================================================================= */
        /* ========================================================================= */
        inline constexpr LetterType a()
        {
            return {
                {0b000000'00,
                 0b000000'00,
                 0b011110'00,
                 0b000001'00,
                 0b111111'00,
                 0b100001'00,
                 0b011111'00}};
        }
        inline constexpr LetterType b()
        {
            return {
                {0b100000'00,
                 0b100000'00,
                 0b100000'00,
                 0b111110'00,
                 0b100001'00,
                 0b100001'00,
                 0b111110'00}};
        }
        inline constexpr LetterType c()
        {
            return {
                {0b000000'00,
                 0b000000'00,
                 0b011111'00,
                 0b100000'00,
                 0b100000'00,
                 0b100000'00,
                 0b011111'00}};
        }
        inline constexpr LetterType d()
        {
            return {
                {0b000001'00,
                 0b000001'00,
                 0b000001'00,
                 0b011111'00,
                 0b100001'00,
                 0b100001'00,
                 0b011111'00}};
        }
        inline constexpr LetterType e()
        {
            return {
                {0b000000'00,
                 0b000000'00,
                 0b011110'00,
                 0b100001'00,
                 0b111111'00,
                 0b100000'00,
                 0b011111'00}};
        }
        inline constexpr LetterType f()
        {
            return {
                {0b000110'00,
                 0b001001'00,
                 0b001000'00,
                 0b111110'00,
                 0b001000'00,
                 0b001000'00,
                 0b001000'00}};
        }
        inline constexpr LetterType g()
        {
            return {
                {0b000000'00,
                 0b000000'00,
                 0b011110'00,
                 0b100001'00,
                 0b011110'00,
                 0b000001'00,
                 0b000001'00,
                 0b011110'00}};
        }
        inline constexpr LetterType h()
        {
            return {
                {0b100000'00,
                 0b100000'00,
                 0b100000'00,
                 0b111110'00,
                 0b100001'00,
                 0b100001'00,
                 0b100001'00}};
        }
        inline constexpr LetterType i()
        {
            return {
                {0b000000'00,
                 0b001000'00,
                 0b000000'00,
                 0b001000'00,
                 0b001000'00,
                 0b001000'00,
                 0b000100'00}};
        }
        inline constexpr LetterType j()
        {
            return {
                {0b000000'00,
                 0b001000'00,
                 0b000000'00,
                 0b001000'00,
                 0b001000'00,
                 0b001000'00,
                 0b001000'00,
                 0b110000'00}};
        }
        inline constexpr LetterType k()
        {
            return {
                {0b100000'00,
                 0b100000'00,
                 0b100000'00,
                 0b101100'00,
                 0b110000'00,
                 0b101100'00,
                 0b100010'00}};
        }
        inline constexpr LetterType l()
        {
            return {
                {0b001000'00,
                 0b001000'00,
                 0b001000'00,
                 0b001000'00,
                 0b001000'00,
                 0b001000'00,
                 0b001000'00}};
        }
        inline constexpr LetterType m()
        {
            return {
                {0b000000'00,
                 0b000000'00,
                 0b111110'00,
                 0b100101'00,
                 0b100101'00,
                 0b100101'00,
                 0b100101'00}};
        }
        inline constexpr LetterType n()
        {
            return {
                {0b000000'00,
                 0b000000'00,
                 0b111110'00,
                 0b100001'00,
                 0b100001'00,
                 0b100001'00,
                 0b100001'00}};
        }
        inline constexpr LetterType o()
        {
            return {
                {0b000000'00,
                 0b000000'00,
                 0b011110'00,
                 0b100001'00,
                 0b100001'00,
                 0b100001'00,
                 0b011110'00}};
        }
        inline constexpr LetterType p()
        {
            return {
                {0b000000'00,
                 0b000000'00,
                 0b001110'00,
                 0b010001'00,
                 0b011110'00,
                 0b010000'00,
                 0b010000'00,
                 0b010000'00}};
        }
        inline constexpr LetterType q()
        {
            return {
                {0b000000'00,
                 0b000000'00,
                 0b011100'00,
                 0b100010'00,
                 0b011110'00,
                 0b000010'00,
                 0b000010'00,
                 0b000011'00}};
        }
        inline constexpr LetterType r()
        {
            return {
                {0b000000'00,
                 0b000000'00,
                 0b011100'00,
                 0b100010'00,
                 0b100000'00,
                 0b100000'00,
                 0b100000'00}};
        }
        inline constexpr LetterType s()
        {
            return {
                {0b000000'00,
                 0b000000'00,
                 0b001110'00,
                 0b010000'00,
                 0b001100'00,
                 0b000010'00,
                 0b011100'00}};
        }
        inline constexpr LetterType t()
        {
            return {
                {0b000000'00,
                 0b000000'00,
                 0b001000'00,
                 0b011110'00,
                 0b001000'00,
                 0b001000'00,
                 0b000100'00}};
        }
        inline constexpr LetterType u()
        {
            return {
                {0b000000'00,
                 0b000000'00,
                 0b000000'00,
                 0b010001'00,
                 0b010001'00,
                 0b010001'00,
                 0b001111'00}};
        }
        inline constexpr LetterType v()
        {
            return {
                {0b000000'00,
                 0b000000'00,
                 0b000000'00,
                 0b010001'00,
                 0b010001'00,
                 0b011011'00,
                 0b001110'00}};
        }
        inline constexpr LetterType w()
        {
            return {
                {0b000000'00,
                 0b000000'00,
                 0b000000'00,
                 0b100001'00,
                 0b101101'00,
                 0b011110'00,
                 0b010010'00}};
        }
        inline constexpr LetterType x()
        {
            return {
                {0b000000'00,
                 0b000000'00,
                 0b100001'00,
                 0b010010'00,
                 0b001100'00,
                 0b010010'00,
                 0b100001'00}};
        }
        inline constexpr LetterType y()
        {
            return {
                {0b000000'00,
                 0b000000'00,
                 0b010001'00,
                 0b010001'00,
                 0b001111'00,
                 0b000100'00,
                 0b011000'00}};
        }
        inline constexpr LetterType z()
        {
            return {
                {0b000000'00,
                 0b000000'00,
                 0b000000'00,
                 0b011110'00,
                 0b000100'00,
                 0b001000'00,
                 0b011110'00}};
        }

        /* ========================================================================= */
        /* ========================================================================= */
        /* ========================================================================= */
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
                 0b100001'00,
                 0b100001'00,
                 0b111110'00,
                 0b100001'00,
                 0b100001'00,
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
                 0b100010'00,
                 0b100001'00,
                 0b100001'00,
                 0b100001'00,
                 0b100010'00,
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
                 0b011110'00,
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

        /* ========================================================================= */
        /* ========================================================================= */
        /* ========================================================================= */
        // non alpha-numeric
        inline constexpr LetterType Space()
        {
            return {};
        }

        /* ========================================================================= */
        /* ========================================================================= */
        /* ========================================================================= */
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
        constexpr auto one_bpp_transfer{[](auto lt) -> LetterType
                                        {
                                            return reverse(invert(std::move(lt)));
                                        }};
        constexpr auto num{[](const char ch)
                           {
                               return static_cast<size_t>(ch) - 32;
                           }};
        static_assert(num(' ') == 0, "charcter SP is not at 32?");
        std::array<LetterType, 6 * 16 - 1> rv{};
        for (auto &c : rv)
        {
            c = one_bpp_transfer(details::Stub());
        }

        rv[num(' ')] = one_bpp_transfer(details::Space());

        rv[num('0')] = one_bpp_transfer(details::Number_Zero());
        rv[num('1')] = one_bpp_transfer(details::Number_One());
        rv[num('2')] = one_bpp_transfer(details::Number_Two());
        rv[num('3')] = one_bpp_transfer(details::Number_Three());
        rv[num('4')] = one_bpp_transfer(details::Number_Four());
        rv[num('5')] = one_bpp_transfer(details::Number_Five());
        rv[num('6')] = one_bpp_transfer(details::Number_Six());
        rv[num('7')] = one_bpp_transfer(details::Number_Seven());
        rv[num('8')] = one_bpp_transfer(details::Number_Eight());
        rv[num('9')] = one_bpp_transfer(details::Number_Nine());

        rv[num('A')] = one_bpp_transfer(details::A());
        rv[num('B')] = one_bpp_transfer(details::B());
        rv[num('C')] = one_bpp_transfer(details::C());
        rv[num('D')] = one_bpp_transfer(details::D());
        rv[num('E')] = one_bpp_transfer(details::E());
        rv[num('F')] = one_bpp_transfer(details::F());
        rv[num('G')] = one_bpp_transfer(details::G());
        rv[num('H')] = one_bpp_transfer(details::H());
        rv[num('I')] = one_bpp_transfer(details::I());
        rv[num('J')] = one_bpp_transfer(details::J());
        rv[num('K')] = one_bpp_transfer(details::K());
        rv[num('L')] = one_bpp_transfer(details::L());
        rv[num('M')] = one_bpp_transfer(details::M());
        rv[num('N')] = one_bpp_transfer(details::N());
        rv[num('O')] = one_bpp_transfer(details::O());
        rv[num('P')] = one_bpp_transfer(details::P());
        rv[num('Q')] = one_bpp_transfer(details::Q());
        rv[num('R')] = one_bpp_transfer(details::R());
        rv[num('S')] = one_bpp_transfer(details::S());
        rv[num('T')] = one_bpp_transfer(details::T());
        rv[num('U')] = one_bpp_transfer(details::U());
        rv[num('V')] = one_bpp_transfer(details::V());
        rv[num('W')] = one_bpp_transfer(details::W());
        rv[num('X')] = one_bpp_transfer(details::X());
        rv[num('Y')] = one_bpp_transfer(details::Y());
        rv[num('Z')] = one_bpp_transfer(details::Z());

        rv[num('a')] = one_bpp_transfer(details::a());
        rv[num('b')] = one_bpp_transfer(details::b());
        rv[num('c')] = one_bpp_transfer(details::c());
        rv[num('d')] = one_bpp_transfer(details::d());
        rv[num('e')] = one_bpp_transfer(details::e());
        rv[num('f')] = one_bpp_transfer(details::f());
        rv[num('g')] = one_bpp_transfer(details::g());
        rv[num('h')] = one_bpp_transfer(details::h());
        rv[num('i')] = one_bpp_transfer(details::i());
        rv[num('j')] = one_bpp_transfer(details::j());
        rv[num('k')] = one_bpp_transfer(details::k());
        rv[num('l')] = one_bpp_transfer(details::l());
        rv[num('m')] = one_bpp_transfer(details::m());
        rv[num('n')] = one_bpp_transfer(details::n());
        rv[num('o')] = one_bpp_transfer(details::o());
        rv[num('p')] = one_bpp_transfer(details::p());
        rv[num('q')] = one_bpp_transfer(details::q());
        rv[num('r')] = one_bpp_transfer(details::r());
        rv[num('s')] = one_bpp_transfer(details::s());
        rv[num('t')] = one_bpp_transfer(details::t());
        rv[num('u')] = one_bpp_transfer(details::u());
        rv[num('v')] = one_bpp_transfer(details::v());
        rv[num('w')] = one_bpp_transfer(details::w());
        rv[num('x')] = one_bpp_transfer(details::x());
        rv[num('y')] = one_bpp_transfer(details::y());
        rv[num('z')] = one_bpp_transfer(details::z());

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