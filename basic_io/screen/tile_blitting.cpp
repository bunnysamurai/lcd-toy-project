#include "tile_blitting.hpp"

#include <cstddef>
#include <cstdint>

#include "TileDef.h"

namespace screen {

void blit_1bpp(uint8_t *__restrict buffer, size_t width, size_t x, size_t y,
               Tile tile) {
  /*
   * much like 4bpp, except there's 4 possible offset cases to handle
   *
   * 4 pixels per byte, so the x offset will make us:
   *
   *   x%8=0
   *   XXXXXXXX YYYYYYYY
   *   01234567
   *
   *   x%8=1
   *   XXXXXXXX YYYYYYYY
   *   -0123456 7
   *
   *   x%8=2
   *   XXXXXXXX YYYYYYYY
   *   --012345 67
   *
   *   x%8=3
   *   XXXXXXXX YYYYYYYY
   *   ---01234 567
   *
   *   x%8=4
   *   XXXXXXXX YYYYYYYY
   *   ----0123 4567
   *
   *   x%8=5
   *   XXXXXXXX YYYYYYYY
   *   -----012 34567
   *
   *   x%8=6
   *   XXXXXXXX YYYYYYYY
   *   ------01 234567
   *
   *   x%8=7
   *   XXXXXXXX YYYYYYYY
   *   -------0 1234567
   *
   */

  auto &&mod0{[&]() {
    /* TODO broken at the moment.  but once it gets working, I feel like the
     * interpolator could help out here. */
    for (size_t yy = 0; yy < tile.side_length; ++yy) {
      for (size_t xx = 0; xx < tile.side_length; xx += 8) {
        const size_t bufidx{(yy + y) * width + (xx + x)};
        const size_t tilidx{yy * tile.side_length + xx};
        buffer[bufidx >> 3] = tile.data[tilidx >> 3];
      }
    }
  }};
  auto &&mod1{[&]() {
    for (size_t yy = 0; yy < tile.side_length; ++yy) {
      for (size_t xx = 0; xx < tile.side_length; xx += 8) {
        const size_t bufidx{((yy + y) * width + (xx + x)) >> 3};
        const size_t tilidx{(yy * tile.side_length + xx) >> 3};
        const auto data{tile.data[tilidx]};

        const auto byte_x0{(data >> 1) & 0b0111'1111};
        const auto byte_x1{(data << 7) & 0b1000'0000};
        buffer[bufidx] |= byte_x0;
        buffer[bufidx + 1] |= byte_x1;
      }
    }
  }};
  auto &&mod2{[&]() {
    for (size_t yy = 0; yy < tile.side_length; ++yy) {
      for (size_t xx = 0; xx < tile.side_length; xx += 8) {
        const size_t bufidx{((yy + y) * width + (xx + x)) >> 3};
        const size_t tilidx{(yy * tile.side_length + xx) >> 3};
        const auto data{tile.data[tilidx]};

        const auto byte_x0{(data >> 2) & 0b0011'1111};
        const auto byte_x1{(data << 6) & 0b1100'0000};
        buffer[bufidx] |= byte_x0;
        buffer[bufidx + 1] |= byte_x1;
      }
    }
  }};
  auto &&mod3{[&]() {
    for (size_t yy = 0; yy < tile.side_length; ++yy) {
      for (size_t xx = 0; xx < tile.side_length; xx += 8) {
        const size_t bufidx{((yy + y) * width + (xx + x)) >> 3};
        const size_t tilidx{(yy * tile.side_length + xx) >> 3};
        const auto data{tile.data[tilidx]};

        const auto byte_x0{(data >> 3) & 0b0001'1111};
        const auto byte_x1{(data << 5) & 0b1110'0000};
        buffer[bufidx] |= byte_x0;
        buffer[bufidx + 1] |= byte_x1;
      }
    }
  }};
  auto &&mod4{[&]() {
    for (size_t yy = 0; yy < tile.side_length; ++yy) {
      for (size_t xx = 0; xx < tile.side_length; xx += 8) {
        const size_t bufidx{((yy + y) * width + (xx + x)) >> 3};
        const size_t tilidx{(yy * tile.side_length + xx) >> 3};
        const auto data{tile.data[tilidx]};

        const auto byte_x0{(data >> 4) & 0b0000'1111};
        const auto byte_x1{(data << 4) & 0b1111'0000};
        buffer[bufidx] |= byte_x0;
        buffer[bufidx + 1] |= byte_x1;
      }
    }
  }};
  auto &&mod5{[&]() {
    for (size_t yy = 0; yy < tile.side_length; ++yy) {
      for (size_t xx = 0; xx < tile.side_length; xx += 8) {
        const size_t bufidx{((yy + y) * width + (xx + x)) >> 3};
        const size_t tilidx{(yy * tile.side_length + xx) >> 3};
        const auto data{tile.data[tilidx]};

        const auto byte_x0{(data >> 5) & 0b0000'0111};
        const auto byte_x1{(data << 3) & 0b1111'1000};
        buffer[bufidx] |= byte_x0;
        buffer[bufidx + 1] |= byte_x1;
      }
    }
  }};
  auto &&mod6{[&]() {
    for (size_t yy = 0; yy < tile.side_length; ++yy) {
      for (size_t xx = 0; xx < tile.side_length; xx += 8) {
        const size_t bufidx{((yy + y) * width + (xx + x)) >> 3};
        const size_t tilidx{(yy * tile.side_length + xx) >> 3};
        const auto data{tile.data[tilidx]};

        const auto byte_x0{(data >> 6) & 0b0000'0011};
        const auto byte_x1{(data << 2) & 0b1111'1100};
        buffer[bufidx] |= byte_x0;
        buffer[bufidx + 1] |= byte_x1;
      }
    }
  }};
  auto &&mod7{[&]() {
    for (size_t yy = 0; yy < tile.side_length; ++yy) {
      for (size_t xx = 0; xx < tile.side_length; xx += 8) {
        const size_t bufidx{((yy + y) * width + (xx + x)) >> 3};
        const size_t tilidx{(yy * tile.side_length + xx) >> 3};
        const auto data{tile.data[tilidx]};

        const auto byte_x0{(data >> 7) & 0b0000'0001};
        const auto byte_x1{(data << 1) & 0b1111'1110};
        buffer[bufidx] |= byte_x0;
        buffer[bufidx + 1] |= byte_x1;
      }
    }
  }};

  const auto bit_offset{(y * width + x) & 0b111};

  switch (bit_offset) {
  case 0:
    mod0();
    break;
  case 1:
    mod1();
    break;
  case 2:
    mod2();
    break;
  case 3:
    mod3();
    break;
  case 4:
    mod4();
    break;
  case 5:
    mod5();
    break;
  case 6:
    mod6();
    break;
  case 7:
    mod7();
    break;
  }
}

void blit_2bpp(uint8_t *__restrict buffer, size_t width, size_t x, size_t y,
               Tile tile) {
  /*
   * much like 4bpp, except there's 4 possible offset cases to handle
   *
   * 4 pixels per byte, so the x offset will make us:
   *
   *   x%4=0
   *   XXXXXXXX YYYYYYYY
   *   p0p1p2p3
   *
   *   x%4=1
   *   XXXXXXXX YYYYYYYY
   *   --p0p1p2 p3
   *
   *   x%4=2
   *   XXXXXXXX YYYYYYYY
   *   ----p0p1 p2p3
   *
   *   x%4=3
   *   XXXXXXXX YYYYYYYY
   *   ------p0 p1p2p3
   *
   */

  auto &&mod0{[&]() {
    for (size_t yy = 0; yy < tile.side_length; ++yy) {
      for (size_t xx = 0; xx < tile.side_length; xx += 4) {
        const size_t bufidx{(yy + y) * width + (xx + x)};
        const size_t tilidx{yy * tile.side_length + xx};
        buffer[bufidx >> 2] = tile.data[tilidx >> 2];
      }
    }
  }};
  auto &&mod1{[&]() {
    for (size_t yy = 0; yy < tile.side_length; ++yy) {
      for (size_t xx = 0; xx < tile.side_length; xx += 4) {
        const size_t bufidx{((yy + y) * width + (xx + x)) >> 2};
        const size_t tilidx{(yy * tile.side_length + xx) >> 2};
        const auto data{tile.data[tilidx]};

        const auto byte_x0{(data >> 2) & 0b0011'1111};
        const auto byte_x1{(data << 6) & 0b1100'0000};
        buffer[bufidx] |= byte_x0;
        buffer[bufidx + 1] |= byte_x1;
      }
    }
  }};
  auto &&mod2{[&]() {
    for (size_t yy = 0; yy < tile.side_length; ++yy) {
      for (size_t xx = 0; xx < tile.side_length; xx += 4) {
        const size_t bufidx{((yy + y) * width + (xx + x)) >> 2};
        const size_t tilidx{(yy * tile.side_length + xx) >> 2};
        const auto data{tile.data[tilidx]};

        const auto byte_x0{(data >> 4) & 0b0000'1111};
        const auto byte_x1{(data << 4) & 0b1111'0000};
        buffer[bufidx] |= byte_x0;
        buffer[bufidx + 1] |= byte_x1;
      }
    }
  }};
  auto &&mod3{[&]() {
    for (size_t yy = 0; yy < tile.side_length; ++yy) {
      for (size_t xx = 0; xx < tile.side_length; xx += 4) {
        const size_t bufidx{((yy + y) * width + (xx + x)) >> 2};
        const size_t tilidx{(yy * tile.side_length + xx) >> 2};
        const auto data{tile.data[tilidx]};

        const auto byte_x0{(data >> 6) & 0b0000'0011};
        const auto byte_x1{(data << 2) & 0b1111'1100};
        buffer[bufidx] |= byte_x0;
        buffer[bufidx + 1] |= byte_x1;
      }
    }
  }};

  const auto nibble_offset{(y * width + x) & 0b11};

  switch (nibble_offset) {
  case 0:
    mod0();
    break;
  case 1:
    mod1();
    break;
  case 2:
    mod2();
    break;
  case 3:
    mod3();
    break;
  }
}

void blit_4bpp(uint8_t *__restrict buffer, size_t width, size_t x, size_t y,
               Tile tile) {
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
  const bool nibbles_match{0 == (x & 0b1)};
  if (nibbles_match) {
    for (size_t yy = 0; yy < tile.side_length; ++yy) {
      for (size_t xx = 0; xx < tile.side_length; xx += 2) {
        const size_t idx{(yy + y) * width + (xx + x)};
        const size_t idx2{yy * tile.side_length + xx};
        buffer[idx >> 1] = tile.data[idx2 >> 1];
      }
    }
  } else {
    for (size_t yy = 0; yy < tile.side_length; ++yy) {
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

void blit_8bpp(uint8_t *__restrict buffer, size_t width, size_t x, size_t y,
               Tile tile) {
  for (size_t yy = 0; yy < tile.side_length; ++yy) {
    for (size_t xx = 0; xx < tile.side_length; ++xx) {
      const size_t idx{(yy + y) * width + (xx + x)};
      const size_t idx2{yy * tile.side_length + xx};
      buffer[idx] = tile.data[idx2];
    }
  }
}

void blit_16bpp(uint8_t *__restrict buffer, size_t width, size_t x, size_t y,
                Tile tile) {
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