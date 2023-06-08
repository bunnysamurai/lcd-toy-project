#ifndef BIT_MANIP_HPP
#define BIT_MANIP_HPP

#include <cstdint>
#include <bitset>

namespace bit_manip
{
    // TODO add concepts to constrain all of these functions
    template <class T>
    [[nodiscard]] constexpr T reverse_bits(T val) noexcept
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
    template <class T>
    constexpr void swap_bits(T &valA, T &valB, size_t posA, size_t posB) noexcept
    {
        /* Truth tables?
         *
         * |  A  |  B  |  OutA  |  OutB  |
         * |-----|-----|--------|--------|
         * |  0  |  0  |   0    |    0   |
         * |  0  |  1  |   1    |    0   |
         * |  1  |  0  |   0    |    1   |
         * |  1  |  1  |   1    |    1   |
         */
        // std::bitset<sizeof(T) * 8> valA_{valA};
        // std::bitset<sizeof(T) * 8> valB_{valB};
        // bool tmp{valA_[posA]};
        // valA_[posA] = valB_[posB];
        // valB_[posB] = tmp;
        // valA = valA_.to_ulong();
        // valB = valB_.to_ulong();

        auto &&select{
            [](const auto &c, size_t pos) -> bool
            {
                return (c >> pos) & 1;
            }};
        auto &&set{[](auto &c, size_t pos, bool v)
                   {
                       if (v)
                       {
                           c |= (1 << pos);
                       }
                       else
                       {
                           c &= ~(1 << pos);
                       }
                   }};
        bool tmp{select(valA, posA)};
        set(valA, posA, select(valB, posB));
        set(valB, posB, tmp);
    }
}

#endif