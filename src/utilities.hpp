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

} // namespace embp

#endif