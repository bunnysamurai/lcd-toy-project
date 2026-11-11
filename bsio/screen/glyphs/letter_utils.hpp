#if !defined(LETTER_UTILS_HPP)
#define LETTER_UTILS_HPP

#include <array>
#include <cstdint>

#include "embp/constexpr_numeric.hpp"

#include "letters.hpp"
#include "../screen.hpp"

namespace screen
{

using letter_4bpp_array_t = std::array<uint8_t, (glyphs::tile::width() / 2) * glyphs::tile::height()>;

constexpr void copy_1bpptile_to_4bpp_buffer(letter_4bpp_array_t &tile_4bpp, const screen::Tile &tile_1bpp,
                                            const uint8_t set_word = 0b1111, const uint8_t unset_word = 0b0000) noexcept
{
    const auto *p_in{tile_1bpp.data};
    auto *p_out{std::data(tile_4bpp)};

    const auto bitlength{tile_1bpp.side_length * tile_1bpp.side_length};

    /* this isn't complicated, just extend each bit in tile_1bpp */
    uint32_t outbyteidx{};
    for (uint32_t bitidx{0}; bitidx < bitlength; bitidx += 8)
    {
        const auto inbtyeidx{bitidx >> 3};
        const auto val{p_in[inbtyeidx]};
        for (uint32_t bit{0}; bit < 8; bit += 2)
        {
            const bool lwrisset{((val >> bit) & 0b1) == 1};
            const bool uprisset{((val >> (bit + 1)) & 0b1) == 1};
            const auto lowernib{set_word * lwrisset + unset_word * (!lwrisset)};
            const auto uppernib{set_word * uprisset + unset_word * (!uprisset)};
            p_out[outbyteidx++] = (uppernib << 4) | lowernib;
        }
    }
}

void get_letter_data_4bpp(letter_4bpp_array_t &output_tile, char character, uint8_t foreground = 0b1111,
                          uint8_t background = 0b0000) noexcept;

void draw_standard_character_to_4bpp_display(char c, uint32_t left_column, uint32_t top_row, uint8_t foreground,
                                             uint8_t background) noexcept;

} // namespace screen

#endif