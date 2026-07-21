#include "rng.hpp"

#include "screen/glyphs/bit_manip.hpp"

#include <pico/rand.h>

#include <cstring>
#include <random>

namespace
{
uint64_t g_seed[2];

[[nodiscard]] constexpr uint32_t lfsr(uint32_t input) noexcept
{
    uint8_t buf[4];
    std::memcpy(buf, &input, sizeof(uint32_t));
    const uint8_t result{static_cast<uint8_t>(buf[0] - buf[3] - static_cast<uint8_t>(g_seed[1] & 0b1))};
    buf[3] = buf[2];
    buf[2] = buf[1];
    buf[1] = buf[0];
    buf[0] = result + ((result & 0x80) != 0) * 256U;
    std::memcpy(&input, buf, sizeof(uint32_t));
    return input;
}

} // namespace

namespace rng
{

void set_seed(int input) noexcept
{
    const uint64_t val{static_cast<uint64_t>(input)};
    g_seed[0] = bit_manip::reverse_bits(val);
    g_seed[1] = input;
}

uint32_t prng() noexcept
{
    if (!g_seed[0] && !g_seed[1])
    {
        rng_128_t data;
        get_rand_128(&data);
        g_seed[0] = data.r[0];
        g_seed[1] = data.r[1];
    }

    uint32_t buf[4];
    std::memcpy(buf, g_seed, sizeof(uint32_t) * 4);

    const uint32_t result{lfsr(buf[0])};

    buf[0] = buf[1];
    buf[1] = buf[2];
    buf[2] = buf[3];
    buf[3] = result;

    std::memcpy(g_seed, buf, sizeof(uint32_t) * 4);

    return result;
}
} // namespace rng