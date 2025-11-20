#if !defined(TILE_HPP)
#define TILE_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>

#include "TileDef.h"
#include "details/tile_manip.hpp"
#include "screen_def.h"

/** @brief A collection of tile buffer manipulation functions */
namespace screen {

inline void copy(uint8_t *out, screen::Tile tile) noexcept {
  memcpy(out, tile.data, tile.side_length * tile.side_length);
}

template <size_t N>
[[nodiscard]] inline bool copy(screen::Tile tile,
                               std::array<uint8_t, N> &outbuf) noexcept {
  if (std::size(outbuf) < tile.side_length * tile.side_length) {
    return false;
  }
  copy(std::data(outbuf), tile);
  return true;
}

template <size_t N>
[[nodiscard]] inline bool
copy_with_replacement(screen::Tile tile, std::array<uint8_t, N> &outbuf,
                      uint32_t pattern, uint32_t replacement) {

  if (pattern == replacement) {
    return copy(tile, outbuf);
  }

  const auto tilelen{
      (tile.side_length * tile.side_length * bitsizeof(tile.format)) >> 3};
  if (std::size(outbuf) < tilelen) {
    return false;
  }

  switch (tile.format) {
  case Format::GREY1:
    details::copy_and_replace_1bpp(tile.data, tilelen, std::data(outbuf),
                                   pattern, replacement);
    break;
  case Format::GREY2:
    details::copy_and_replace_2bpp(tile.data, tilelen, std::data(outbuf),
                                   pattern, replacement);
    break;
  case Format::GREY4:
  case Format::RGB565_LUT4:
    details::copy_and_replace_4bpp(tile.data, tilelen, std::data(outbuf),
                                   pattern, replacement);
    break;
  case Format::RGB565_LUT8:
    details::copy_and_replace_8bpp(tile.data, tilelen, std::data(outbuf),
                                   pattern, replacement);
    break;
  case Format::RGB565:
    details::copy_and_replace_16bpp(tile.data, tilelen, std::data(outbuf),
                                    pattern, replacement);
    break;
  }
  return true;
}

} // namespace screen

#endif
