#if !defined(BITIMAGE_HPP)
#define BITIMAGE_HPP

#include <array>
#include <cstdint>

template <uint32_t NCols, uint32_t NRows> struct BitImage {
public:
  std::array<uint32_t, (NCols * NRows + 31) / (8 * 4)> m_field{};

  [[nodiscard]] constexpr bool get(uint32_t x, uint32_t y) const noexcept {
    const auto [idx, rem]{to_idx(x, y)};
    return to_bit(m_field[idx], rem);
  }
  constexpr void set(bool value, uint32_t x, uint32_t y) noexcept {
    const auto [idx, rem]{to_idx(x, y)};
    set_bit(m_field[idx], rem, value);
  }

  static constexpr void set_bit(uint32_t &word, uint32_t rem,
                                bool value) noexcept {
    if (value) {
      word |= (1 << rem);
    } else {
      word &= ~(1 << rem);
    }
  }
  [[nodiscard]] static constexpr bool to_bit(uint32_t word,
                                             uint32_t rem) noexcept {
    return (word >> rem) & 0b1;
  }

  [[nodiscard]] static constexpr std::pair<uint32_t, uint32_t>
  to_idx(uint32_t x, uint32_t y) noexcept {
    const uint32_t major{(y * NCols + x) >> 5};
    const uint32_t minor{(y * NCols + x) - (major << 5)};
    return std::make_pair(major, minor);
  }
};

namespace constexpr_tests {
[[nodiscard]] constexpr bool test_bitimage() {
  BitImage<10, 20> dut{};

  bool result{true};

  /* first, confirm all the bits are set to 0 */
  for (uint32_t yy = 0; yy < 20; ++yy) {
    for (uint32_t xx = 0; xx < 10; ++xx) {
      result &= dut.get(xx, yy) == 0;
    }
  }

  /* second, confirm set-get pattern */
  for (uint32_t yy = 0; yy < 20; ++yy) {
    for (uint32_t xx = 0; xx < 10; ++xx) {
      for (uint8_t testval = 0; testval < 0b1; ++testval) {
        dut.set(testval, xx, yy);
        result &= dut.get(xx, yy) == testval;
      }
    }
  }

  return result;
}

static_assert(test_bitimage());
} // namespace constexpr_tests

#endif