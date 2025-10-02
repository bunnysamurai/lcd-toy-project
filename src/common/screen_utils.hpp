#if !defined(SCREEN_UTILS_HPP)
#define SCREEN_UTILS_HPP

#include <array>
#include <cstdint>

#include "embp/constexpr_numeric.hpp"

#include "screen/glyphs/letters.hpp"
#include "screen/screen.hpp"

namespace screen {

using letter_4bpp_array_t =
    std::array<uint8_t, (glyphs::tile::width() / 2) * glyphs::tile::height()>;

constexpr void
copy_1bpptile_to_4bpp_buffer(letter_4bpp_array_t &tile_4bpp,
                             const screen::Tile &tile_1bpp,
                             const uint8_t set_word = 0b1111,
                             const uint8_t unset_word = 0b0000) noexcept {
  const auto *p_in{tile_1bpp.data};
  auto *p_out{std::data(tile_4bpp)};

  const auto bitlength{tile_1bpp.side_length * tile_1bpp.side_length};

  /* this isn't complicated, just extend each bit in tile_1bpp */
  uint32_t outbyteidx{};
  for (uint32_t bitidx{0}; bitidx < bitlength; bitidx += 8) {
    const auto inbtyeidx{bitidx >> 3};
    const auto val{p_in[inbtyeidx]};
    for (uint32_t bit{0}; bit < 8; bit += 2) {
      const bool lwrisset{((val >> bit) & 0b1) == 1};
      const bool uprisset{((val >> (bit + 1)) & 0b1) == 1};
      const auto lowernib{set_word * lwrisset + unset_word * (!lwrisset)};
      const auto uppernib{set_word * uprisset + unset_word * (!uprisset)};
      p_out[outbyteidx++] = (uppernib << 4) | lowernib;
    }
  }
}

void get_letter_data_4bpp(letter_4bpp_array_t &output_tile, char character,
                          uint8_t foreground = 0b1111,
                          uint8_t background = 0b0000) noexcept;

/** @brief Byte Coded Decimal
 *
 *  Convert integral to array of decimal digits.
 *  Decimal digit array will be in most-significant digit to least-significant
 *  ordering.
 *
 *  For example, if value == 12345, then a call of:
 *    bcd<5>(12345)
 *  will yield the equivalent array:
 *    std::array<uint8_t,5>{5,4,3,2,1};
 *
 * @param value Value to convert.
 *
 * @return Said array
 */
template <size_t N>
[[nodiscard]] constexpr std::array<uint8_t, N> bcd(uint32_t value) noexcept {
  std::array<uint8_t, N> digits{};
  uint32_t start{digits.size()};
  uint32_t score{value};
  uint32_t compare_value{embp::power_intexp(10, N - 1)};
  while (start > 1) {
    const auto idx{(start - 1)};
    while (score >= compare_value) {
      score -= compare_value;
      ++digits[digits.size() - start];
    }
    --start;
    compare_value /= 10;
  }
  digits.back() = score;

  return digits;
}

static_assert(bcd<2>(0)[0] == 0);
static_assert(bcd<2>(1)[0] == 0);
static_assert(bcd<2>(2)[0] == 0);
static_assert(bcd<2>(3)[0] == 0);
static_assert(bcd<2>(4)[0] == 0);
static_assert(bcd<2>(5)[0] == 0);
static_assert(bcd<2>(6)[0] == 0);
static_assert(bcd<2>(7)[0] == 0);
static_assert(bcd<2>(8)[0] == 0);
static_assert(bcd<2>(9)[0] == 0);
static_assert(bcd<2>(10)[0] == 1);

static_assert(bcd<2>(0)[1] == 0);
static_assert(bcd<2>(1)[1] == 1);
static_assert(bcd<2>(2)[1] == 2);
static_assert(bcd<2>(3)[1] == 3);
static_assert(bcd<2>(4)[1] == 4);
static_assert(bcd<2>(5)[1] == 5);
static_assert(bcd<2>(6)[1] == 6);
static_assert(bcd<2>(7)[1] == 7);
static_assert(bcd<2>(8)[1] == 8);
static_assert(bcd<2>(9)[1] == 9);
static_assert(bcd<2>(10)[1] == 0);

static_assert(bcd<6>(345612)[0] == 3);
static_assert(bcd<6>(345612)[1] == 4);
static_assert(bcd<6>(345612)[2] == 5);
static_assert(bcd<6>(345612)[3] == 6);
static_assert(bcd<6>(345612)[4] == 1);
static_assert(bcd<6>(345612)[5] == 2);

} // namespace screen

#endif