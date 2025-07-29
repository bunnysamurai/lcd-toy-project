
#include <iostream>

#include <cstddef>
#include <cstdint>

#include <algorithm>
#include <array>

#include "TileDef.h"
#include "tile_blitting.hpp"

using screen::Format;
using screen::Tile;

namespace tests {

static constexpr bool PRINT_DEBUG{true};

/* blit_4bpp */
[[nodiscard]] bool test_4bpp() noexcept {

  bool status{true};

  /* ODD tile is 3x3 of alternating 1/0's per nibble */
  std::array<uint8_t, 6> odd_tile_data{0b10101101, 0b00000101, 0b10100101,
                                       0b00000101, 0b10100101, 0b00001101};
  Tile odd_tile{.side_length = 3,
                .format = Format::RGB565_LUT4,
                .data = odd_tile_data.data()};
  /* EVEN tile is 4x4 of alternating 1/0's per nibble */
  std::array<uint8_t, 8> even_tile_data{0b10100111, 0b10100101, 0b10100101,
                                        0b10100101, 0b10100101, 0b10100101,
                                        0b10100101, 0b10100111};
  Tile even_tile{.side_length = 4,
                 .format = Format::RGB565_LUT4,
                 .data = even_tile_data.data()};

  /* our video buffer is 6x6 pixels */
  auto &&create_video_buffer{[]() { return std::array<uint8_t, 6 * 6 / 2>{}; }};

  /* our test cases are:
   *  even tile at {x,y} 0,0
   *  even tile at {x,y} 1,0
   *  odd tile at {x,y} 0,0
   *  odd tile at {x,y} 1,0
   */
  auto &&test_apparatus{[&](const auto expected, const auto tile,
                            const auto xpos, const auto ypos) {
    auto vidbuf{create_video_buffer()};
    blit_4bpp(vidbuf.data(), 6, xpos, ypos, tile);

    const bool result{std::equal(std::begin(expected), std::end(expected),
                                 std::begin(vidbuf), std::end(vidbuf))};

    if (!result && PRINT_DEBUG) {
      auto &&print_array{[](const auto arr, const auto msg) {
        std::cerr << msg << " { ";
        for (const auto c : arr) {
          std::cerr << +c << ' ';
        }
        std::cerr << "}\n";
      }};

      std::cerr << "test_4bpp, " << xpos << ", " << ypos
                << ", sidelength = " << +tile.side_length << '\n';
      print_array(expected, "  expected");
      print_array(vidbuf, "  vidbuf  ");
    }

    return result;
  }};
  {
    /* even tile at {x,y} 0,0 */
    const auto expected1{[&]() {
      auto buf{create_video_buffer()};
      buf[0] = even_tile_data[0];
      buf[1] = even_tile_data[1];

      buf[3] = even_tile_data[2];
      buf[4] = even_tile_data[3];

      buf[6] = even_tile_data[4];
      buf[7] = even_tile_data[5];

      buf[9] = even_tile_data[6];
      buf[10] = even_tile_data[7];

      return buf;
    }()};
    status &= test_apparatus(expected1, even_tile, 0, 0);

    /* even tile at {x,y} 1,0 */
    const auto expected2{[&]() {
      /* EVEN tile is 4x4 of alternating 1/0's per nibble */
      auto buf{create_video_buffer()};
      // 0,0 of tile -> 1,0 of vid
      // 1,0 of tile -> 2,0 of vid
      // 2,0 of tile -> 3,0 of vid
      // 3,0 of tile -> 4,0 of vid
      buf[0] |= even_tile_data[0] << 4;
      buf[1] = even_tile_data[0] >> 4 | even_tile_data[1] << 4;
      buf[2] |= even_tile_data[1] >> 4;

      buf[3] |= even_tile_data[2] << 4;
      buf[4] = even_tile_data[2] >> 4 | even_tile_data[3] << 4;
      buf[5] |= even_tile_data[3] >> 4;

      buf[6] |= even_tile_data[4] << 4;
      buf[7] = even_tile_data[4] >> 4 | even_tile_data[5] << 4;
      buf[8] |= even_tile_data[5] >> 4;

      buf[9] |= even_tile_data[6] << 4;
      buf[10] = even_tile_data[6] >> 4 | even_tile_data[7] << 4;
      buf[11] |= even_tile_data[7] >> 4;

      return buf;
    }()};
    status &= test_apparatus(expected2, even_tile, 1, 0);

    /* odd tile at {x,y} 0,0 */
    const auto expected3{[&]() {
      /* ODD tile is 3x3 of alternating 1/0's per nibble */
      auto buf{create_video_buffer()};

      /* Tile layout in memory, note rows are byte-aligned
       *  0000 1111 2222 xxxx
       *  3333 4444 5555 xxxx
       *  6666 7777 8888 xxxx
       */
      buf[0] = odd_tile_data[0];
      buf[1] |= odd_tile_data[1] & 0b1111;

      buf[3] = odd_tile_data[2];
      buf[4] = odd_tile_data[3] & 0b1111;

      buf[6] = odd_tile_data[4];
      buf[7] = odd_tile_data[5] & 0b1111;

      return buf;
    }()};
    status &= test_apparatus(expected3, odd_tile, 0, 0);

    /* odd tile at {x,y} 1,0 */
    const auto expected4{[&]() {
      /* ODD tile is 3x3 of alternating 1/0's per nibble */
      auto buf{create_video_buffer()};

      /* Tile layout in memory, note rows are byte-aligned
       * nb/row  0    1    2    3    4
       *   0    ---- 0000 1111 2222 xxxx
       *   1    ---- 3333 4444 5555 xxxx
       *   2    ---- 6666 7777 8888 xxxx
       */
      buf[0] |= (odd_tile_data[0] & 0b1111) << 4;
      buf[1] |= (odd_tile_data[0] >> 4) & 0b1111;
      buf[1] |= (odd_tile_data[1] & 0b1111) << 4;

      buf[3] |= (odd_tile_data[2] & 0b1111) << 4;
      buf[4] |= (odd_tile_data[2] >> 4) & 0b1111;
      buf[4] |= (odd_tile_data[3] & 0b1111) << 4;

      buf[6] |= (odd_tile_data[4] & 0b1111) << 4;
      buf[7] |= (odd_tile_data[4] >> 4) & 0b1111;
      buf[7] |= (odd_tile_data[5] & 0b1111) << 4;
      return buf;
    }()};
    status &= test_apparatus(expected4, odd_tile, 1, 0);
  }

  return status;
};

} // namespace tests
int main() {
  bool status{true};

  status &= tests::test_4bpp();
  if (!status) {
    std::cerr << "test_4bpp failed!\n";
  }

  if (status) {
    std::cerr << "All tests passed!\n";
  }
}