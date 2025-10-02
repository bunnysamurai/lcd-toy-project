#if !defined(UTILITIES_HPP)
#define UTILITIES_HPP

#include <array>
#include <cstddef>
#include <cstdint>

namespace embp {

template <size_t N>
[[nodiscard]] constexpr auto concat(std::array<uint8_t, N> data) noexcept {
  return data;
}

template <size_t M, size_t... Sizes>
[[nodiscard]] constexpr std::array<uint8_t, M + (Sizes + ...)>
concat(std::array<uint8_t, M> lhs,
       std::array<uint8_t, Sizes>... arrs) noexcept {
  auto remaining{concat(arrs...)};
  std::array<uint8_t, M + (Sizes + ...)> result;

  auto itr{result.begin()};

  for (auto c : lhs) {
    *itr++ = c;
  }
  for (auto c : remaining) {
    *itr++ = c;
  }

  return result;
}

template <class... Args>
[[nodiscard]] constexpr std::array<uint8_t, (sizeof...(Args) + 1) / 2>
pfold(Args... args) noexcept {
  const std::array<uint8_t, sizeof...(args)> ops{static_cast<uint8_t>(args)...};
  std::array<uint8_t, (sizeof...(Args) + 1) / 2> result;
  for (size_t ii = 0; ii < result.size() - 1; ++ii) {
    result[ii] = (ops[ii * 2] | (ops[ii * 2 + 1] << 4));
  }
  const size_t idx{result.size() - 1};
  if constexpr ((ops.size() & 0b1) == 0) {
    result[idx] = (ops[idx * 2] | (ops[idx * 2 + 1] << 4));
  } else {
    result[idx] = ops[idx * 2];
  }
  return result;
}
namespace constexpr_tests {
inline constexpr auto even_result{pfold(0b1, 0b11, 0b1, 0b11)};
inline constexpr auto even_expect{std::array<uint8_t, 2>{0b110001, 0b110001}};
static_assert(std::equal(std::begin(even_expect), std::end(even_expect),
                         std::begin(even_result)));
inline constexpr auto odd_result{pfold(0b1, 0b11, 0b1)};
inline constexpr auto odd_expect{std::array<uint8_t, 2>{0b110001, 0b000001}};
static_assert(std::equal(std::begin(odd_expect), std::end(odd_expect),
                         std::begin(odd_result)));
} // namespace constexpr_tests

} // namespace embp

#endif