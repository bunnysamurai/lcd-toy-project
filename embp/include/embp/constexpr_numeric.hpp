#pragma once
#include "utility.hpp"
#ifdef STD_LIB_AVAILABLE
#include <cstddef>
#else
#include <stddef.h>
#endif

namespace embp {
template <class InputIterator, class InitType>
constexpr inline InitType accumulate(InputIterator first, InputIterator last,
                                     InitType init) {
  // could make concept checks here, but we won't until C++20 is widely
  // available
  for (; first != last; ++first) {
    init = embp::move(init) + *first;
  }
  return init;
}

template <class InputIterator, class InitType, class BinaryOperator>
constexpr inline InitType accumulate(InputIterator first, InputIterator last,
                                     InitType init, BinaryOperator binary_op) {
  // could make concept checks here, but we won't until C++20 is widely
  // available
  for (; first != last; ++first) {
    init = binary_op(embp::move(init), *first);
  }
  return init;
}

/** @brief Raise to a positive integer
 */
[[nodiscard]] constexpr size_t power_intexp(size_t base, size_t exp) noexcept {
  if (exp < 1) {
    return 1;
  }

  size_t result{base};
  while (exp > 1) {
    result *= base;
    --exp;
  }
  return result;
}

static_assert(power_intexp(10, 0) == 1);
static_assert(power_intexp(10, 1) == 10);
static_assert(power_intexp(10, 2) == 10 * 10);
static_assert(power_intexp(10, 9) ==
              10 * 10 * 10 * 10 * 10 * 10 * 10 * 10 * 10);

#ifdef STD_LIB_AVAILABLE
#include <array>
template <class T, size_t N>
[[nodiscard]] constexpr std::array<T, N> filled(T value) noexcept {
  std::array<T, N> result;
  for (auto &c : result) {
    c = value;
  }
  return result;
}
#endif
} // namespace embp
