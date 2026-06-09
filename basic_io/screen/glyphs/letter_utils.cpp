#include "letter_utils.hpp"

#include "../TileDef.h"
#include "../screen.hpp"

namespace screen
{

void get_letter_data_4bpp(letter_4bpp_array_t &output_tile, char character, uint8_t foreground,
                          uint8_t background) noexcept
{
    const auto tile_1bpp{glyphs::tile::decode_ascii(character)};
    copy_1bpptile_to_4bpp_buffer(output_tile, tile_1bpp, foreground, background);
}

void draw_standard_character_to_4bpp_display(char character, uint32_t left_column, uint32_t top_row, uint8_t foreground,
                                             uint8_t background) noexcept
{
    const auto fmt{screen::get_format()};

    letter_4bpp_array_t char_data{};

    screen::Tile tile{glyphs::tile::decode_ascii(character)};

    copy_1bpptile_to_4bpp_buffer(char_data, tile, foreground, background);

    tile.format = fmt;

    tile.data = std::data(char_data);

    screen::draw_tile(left_column, top_row, tile);
}

} // namespace screen