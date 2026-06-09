#if !defined(SCREEN_UTILS_HPP)
#define SCREEN_UTILS_HPP

#include <array>
#include <cstdint>

#include "embp/constexpr_numeric.hpp"

namespace screen
{

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
template <size_t N> [[nodiscard]] constexpr std::array<uint8_t, N> bcd(uint32_t value) noexcept
{
    std::array<uint8_t, N> digits{};
    uint32_t start{digits.size()};
    uint32_t score{value};
    uint32_t compare_value{embp::power_intexp(10, N - 1)};
    while (start > 1)
    {
        const auto idx{(start - 1)};
        while (score >= compare_value)
        {
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