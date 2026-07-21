#ifndef COMMON_RNG_HPP
#define COMMON_RNG_HPP

#include <cstdint>

namespace rng
{

void set_seed(int) noexcept;
[[nodiscard]] uint32_t prng() noexcept;

} // namespace rng

#endif