#if !defined(BITIMAGE_HPP)
#define BITIMAGE_HPP

#include <array>
#include <cstdint>

/** @brief 2-D addressable matrix of 1-bit fields
 *
 */
template <uint32_t NCols, uint32_t NRows> struct BitImage {
public:
  static constexpr uint32_t ROWS{NRows};
  static constexpr uint32_t COLS{NCols};
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

/** @brief 2-D addressable matrix of 2-bit fields
 *   "shave-and a-haircut..."
 */
template <uint32_t NCols, uint32_t NRows> struct TwoBitImage {
public:
  static constexpr uint32_t ROWS{NRows};
  static constexpr uint32_t COLS{NCols};
  std::array<BitImage<NCols, NRows>, 2> m_field;

  [[nodiscard]] constexpr uint8_t get(uint32_t x, uint32_t y) const noexcept {
    return (uint8_t{m_field[1].get(x, y)} << 1) | uint8_t{m_field[0].get(x, y)};
  }
  constexpr void set(uint8_t value, uint32_t x, uint32_t y) noexcept {
    m_field[1].set(((value >> 1) & 0b1) == 1, x, y);
    m_field[0].set((value & 0b1) == 1, x, y);
  }
};

/** @brief 2-D addressable matrix of 3-bit fields
 *
 */
template <uint32_t NCols, uint32_t NRows> struct ThreeBitImage {
public:
  static constexpr uint32_t ROWS{NRows};
  static constexpr uint32_t COLS{NCols};
  std::array<BitImage<NCols, NRows>, 3> m_field;

  [[nodiscard]] constexpr uint8_t get(uint32_t x, uint32_t y) const noexcept {
    // uint32_t count{};
    // uint8_t result{};
    // for (const auto &bit : m_field) {
    //   result |= bit.get(x, y) << count;
    //   ++count;
    // }

    return (uint8_t{m_field[2].get(x, y)} << 2) |
           (uint8_t{m_field[1].get(x, y)} << 1) | uint8_t{m_field[0].get(x, y)};
  }
  constexpr void set(uint8_t value, uint32_t x, uint32_t y) noexcept {
    // uint32_t count{};
    // for (auto &bit : m_field) {
    //   bit.set((value >> count) & 0b1, x, y);
    //   ++count;
    // }

    m_field[2].set(((value >> 2) & 0b1) == 1, x, y);
    m_field[1].set(((value >> 1) & 0b1) == 1, x, y);
    m_field[0].set((value & 0b1) == 1, x, y);
  }
};

/* ========================================================================== */
/*                     _____ _____ ____ _____ ____                            */
/*                    |_   _| ____/ ___|_   _/ ___|                           */
/*                      | | |  _| \___ \ | | \___ \                           */
/*                      | | | |___ ___) || |  ___) |                          */
/*                      |_| |_____|____/ |_| |____/                           */
/*                                                                            */
/* ========================================================================== */
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
[[nodiscard]] constexpr bool test_2bitimage() {
  TwoBitImage<10, 20> dut{};

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
      for (uint8_t testval = 0; testval < 0b11; ++testval) {
        dut.set(testval, xx, yy);
        result &= dut.get(xx, yy) == testval;
      }
    }
  }

  return result;
}
[[nodiscard]] constexpr bool test_3bitimage() {
  ThreeBitImage<10, 20> dut{};

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
      for (uint8_t testval = 0; testval < 0b111; ++testval) {
        dut.set(testval, xx, yy);
        result &= dut.get(xx, yy) == testval;
      }
    }
  }

  return result;
}

static_assert(test_bitimage());
static_assert(test_2bitimage());
} // namespace constexpr_tests

#endif