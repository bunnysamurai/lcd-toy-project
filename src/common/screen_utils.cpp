#include "screen_utils.hpp"

namespace screen {

void get_letter_data_4bpp(letter_4bpp_array_t &output_tile, char character,
                          uint8_t foreground, uint8_t background) noexcept {
  const auto tile_1bpp{glyphs::tile::decode_ascii(character)};
  copy_1bpptile_to_4bpp_buffer(output_tile, tile_1bpp, foreground, background);
}

} // namespace screen