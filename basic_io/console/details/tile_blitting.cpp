#include "tile_blitting.hpp"

#include <cstddef>
#include <cstdint>

#include "../TileDef.h"

namespace tile {

void blit_4bpp(uint8_t *__restrict buffer, size_t width, size_t x, size_t y, Tile tile) {
  /* lsn is pixel 0, msn is pixel 1, etc
   * this only applies to columns, not rows
   * so, if the index is even, we use the lsn
   * otherwise, the msn
   *
   * we always index through the tile in a consistent manner
   *
   * however, this isn't guarenteed for the video buffer,
   * due to the x, y offset requested.
   *
   * this leads to 2 possible states:
   *    #1 -> the nibbles to read-from and assign-to are the same
   *    #2 -> the nibbles are NOT the same
   * we are in state #1 if idx is even
   * we are in state #2 otherwise
   */
  const bool nibbles_match{0 == ((y * width + x) & 0b1)};
  if (nibbles_match) {
    for (size_t yy = 0; yy < tile.side_length; yy += 2) {
      for (size_t xx = 0; xx < tile.side_length; xx += 2) {
        const size_t idx{(yy + y) * width + (xx + x)};
        const size_t idx2{yy * tile.side_length + xx};
        buffer[idx >> 1] = tile.data[idx2 >> 1];
      }
    }
  } else {
    for (size_t yy = 0; yy < tile.side_length; yy += 2) {
      for (size_t xx = 0; xx < tile.side_length; xx += 2) {
        const size_t idx{(yy + y) * width + (xx + x)};
        const size_t idx2{yy * tile.side_length + xx};
        const uint8_t lsn{static_cast<uint8_t>(tile.data[idx2 >> 1] & 0b1111U)};
        const uint8_t msn{
            static_cast<uint8_t>(((tile.data[idx2 >> 1]) >> 4) & 0b1111U)};
        buffer[idx >> 1] |= msn;
        buffer[(idx >> 1) + 1] |= lsn;
      }
    }
  }
}

void blit_8bpp(uint8_t *__restrict  buffer, size_t width, size_t x, size_t y, Tile tile) {
  for (size_t yy = 0; yy < tile.side_length; ++yy) {
    for (size_t xx = 0; xx < tile.side_length; ++xx) {
      const size_t idx{(yy + y) * width + (xx + x)};
      const size_t idx2{yy * tile.side_length + xx};
      buffer[idx] = tile.data[idx2];
    }
  }
}

void blit_16bpp(uint8_t *__restrict buffer, size_t width, size_t x, size_t y, Tile tile) {
  for (size_t yy = 0; yy < tile.side_length; ++yy) {
    for (size_t xx = 0; xx < tile.side_length; ++xx) {
      const size_t idx{((yy + y) * width + (xx + x)) * 2};
      const size_t idx2{(yy * tile.side_length + xx) * 2};
      buffer[idx] = tile.data[idx2];
      buffer[idx + 1] = tile.data[idx2 + 1];
    }
  }
}

} // namespace tile