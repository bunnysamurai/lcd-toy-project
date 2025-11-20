#include "tile_blitting.hpp"

#include <cstddef>
#include <cstdint>

#include <algorithm>
#include <array>

#include "TileDef.h"

// #define PRINT_DEBUG

#ifdef PRINT_DEBUG
#include <iostream>
#endif

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
   *
   * Also, I sort of forgot, if tile.side_length is NOT an even multiple of 2,
   * then we also need to handle the offset
   */
  const bool nibbles_match{0 == (x & 0b1)};
  const int rem{tile.side_length & 0b1};
  const int tile_pitch{tile.side_length + rem};
  if (nibbles_match) {
#ifdef PRINT_DEBUG
    std::cerr << "nibbles match\n";
#endif
    static_assert(sizeof(size_t) == 4);
    size_t yy = 0;
    size_t xx = 0;
    for (; yy < tile.side_length; ++yy) {
      for (; xx < tile.side_length - rem; xx += 2) {
        const size_t idx{(yy + y) * width + (xx + x)};
        const size_t idx2{yy * tile_pitch + xx};
        buffer[idx >> 1] = tile.data[idx2 >> 1];
      }
      if (rem) {
#ifdef PRINT_DEBUG
        std::cerr << "rem is " << rem << '\n';
#endif
        const size_t idx{(yy + y) * width + (xx + x + rem)};
        const size_t idx2{yy * tile_pitch + xx + rem};
        buffer[idx >> 1] &= 0b11110000;
        buffer[idx >> 1] |= (tile.data[idx2 >> 1] & 0b00001111);
      }
      xx = 0;
    }
  } else {
#ifdef PRINT_DEBUG
    std::cerr << "nibbles DONT match\n";
#endif
    size_t yy = 0;
    size_t xx = 0;
    for (; yy < tile.side_length; ++yy) {
#ifdef PRINT_DEBUG
      std::cerr << "row " << yy << '\n';
#endif
      for (; xx < tile.side_length - rem; xx += 2) {
#ifdef PRINT_DEBUG
        std::cerr << "col " << xx << '\n';
#endif
        const size_t idx{(yy + y) * width + (xx + x)};
        const size_t idx2{yy * tile_pitch + xx};
        const uint8_t lsn{static_cast<uint8_t>(tile.data[idx2 >> 1] & 0b1111U)};
        const uint8_t msn{
            static_cast<uint8_t>(((tile.data[idx2 >> 1]) >> 4) & 0b1111U)};
        buffer[idx >> 1] &= 0b00001111;
        buffer[idx >> 1] |= (lsn << 4);
        buffer[(idx >> 1) + 1] &= 0b11110000;
        buffer[(idx >> 1) + 1] |= msn;
      }
      if (rem) {
#ifdef PRINT_DEBUG
        std::cerr << "rem is " << rem << '\n';
        std::cerr << "col " << xx + rem << '\n';
#endif
        const size_t idx{
            (yy + y) * width +
            (xx + x)}; /* off by one error, which are rare for me... why?? */
        const size_t idx2{yy * tile_pitch + xx + rem};
#ifdef PRINT_DEBUG
        std::cerr << "vidbuf idx = " << idx << '\n';
        std::cerr << "tile   idx = " << idx2 << '\n';
#endif
        const uint8_t lsn{static_cast<uint8_t>(tile.data[idx2 >> 1] & 0b1111U)};
        buffer[idx >> 1] &= 0b00001111;
        buffer[idx >> 1] |= (lsn << 4);
      }
      xx = 0;
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

} // namespace screen