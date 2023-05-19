#ifndef BIT_MANIP_HPP
#define BIT_MANIP_HPP

#include <cstdint>

namespace bit_manip
{
    // TODO add concepts to constrain
    template <class T>
    constexpr T reverse_bits(T val)
    {
        T out{};
        constexpr auto EXCEPT_THE_LAST{sizeof(T) * 8 - 1};
        for (uint ii = 0; ii < EXCEPT_THE_LAST; ++ii)
        {
            out = (out + ((val >> ii) & 0x01)) << 1;
        }
        out = (out + ((val >> EXCEPT_THE_LAST) & 0x01));
        return out;
    }
}

#endif