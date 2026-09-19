#pragma once
#include "utility.hpp"
#ifdef STD_LIB_AVAILABLE
#include <cstddef>
#include <algorithm>
#include <functional>
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

/* TODO check template type arg is actual an integral type */
template <class Integer>
constexpr void adjust_with_clamp(Integer& val, Integer amount, Integer lo, Integer hi) noexcept
{
#ifdef STD_LIB_AVAILABLE
  val = std::clamp(std::plus<Integer>{}(val , amount), lo, hi);
#else
  /* TODO not a real impl, as overflow isn't being handled */
  const auto v {val + amount};
  if(v < lo)
  {
    val = lo;
    return;
  }
  if(v > hi)
  {
    val = hi;
    return;    
  }
  val = v;
#endif
}

namespace constexpr_testing{
[[nodiscard]] constexpr bool test_adjust_with_clamp() noexcept
{
  bool result{true};
  int input{42};

  adjust_with_clamp(input, 1, 41, 43); /* input is 42 before call */
  result &= input == 43;
  adjust_with_clamp(input, 1, 41, 43); /* input is 43 before call */
  result &= input == 43;
  adjust_with_clamp(input, -1, 41, 43); /* input is 43 before call */
  result &= input == 42;
  adjust_with_clamp(input, -1, 41, 43); /* input is 42 before call */
  result &= input == 41;
  adjust_with_clamp(input, -1, 41, 43); /* input is 41 before call */
  result &= input == 41;

  return result;
}

static_assert(test_adjust_with_clamp());
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
