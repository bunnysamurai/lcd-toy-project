#include "tile_blitting.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>

#include "TileDef.h"

// #define PRINT_DEBUG

#ifdef PRINT_DEBUG
#include <iostream>
#endif

namespace screen
{

void blit_1bpp(uint8_t *__restrict buffer, uint32_t width, uint32_t x, uint32_t y, Tile tile) noexcept
{
    /*
     * much like 4bpp, except there's 4 possible offset cases to handle
     *
     * 4 pixels per byte, so the x offset will make us:
     *
     *   x%8=0
     *   XXXXXXXX YYYYYYYY
     *   01234567
     *
     *   x%8=1
     *   XXXXXXXX YYYYYYYY
     *   -0123456 7
     *
     *   x%8=2
     *   XXXXXXXX YYYYYYYY
     *   --012345 67
     *
     *   x%8=3
     *   XXXXXXXX YYYYYYYY
     *   ---01234 567
     *
     *   x%8=4
     *   XXXXXXXX YYYYYYYY
     *   ----0123 4567
     *
     *   x%8=5
     *   XXXXXXXX YYYYYYYY
     *   -----012 34567
     *
     *   x%8=6
     *   XXXXXXXX YYYYYYYY
     *   ------01 234567
     *
     *   x%8=7
     *   XXXXXXXX YYYYYYYY
     *   -------0 1234567
     *
     */

    auto &&mod0{[&]() {
        for (uint32_t yy = 0; yy < tile.side_length; ++yy)
        {
            for (uint32_t xx = 0; xx < tile.side_length; xx += 8)
            {
                const uint32_t bufidx{(yy + y) * width + (xx + x)};
                const uint32_t tilidx{yy * tile.side_length + xx};
                buffer[bufidx >> 3] = tile.data[tilidx >> 3];
            }
        }
    }};
    auto &&mod1{[&]() {
        for (uint32_t yy = 0; yy < tile.side_length; ++yy)
        {
            for (uint32_t xx = 0; xx < tile.side_length; xx += 8)
            {
                const uint32_t bufidx{((yy + y) * width + (xx + x)) >> 3};
                const uint32_t tilidx{(yy * tile.side_length + xx) >> 3};
                const auto data{tile.data[tilidx]};

                const auto byte_x0{(data >> 1) & 0b0111'1111};
                const auto byte_x1{(data << 7) & 0b1000'0000};
                buffer[bufidx] |= byte_x0;
                buffer[bufidx + 1] |= byte_x1;
            }
        }
    }};
    auto &&mod2{[&]() {
        for (uint32_t yy = 0; yy < tile.side_length; ++yy)
        {
            for (uint32_t xx = 0; xx < tile.side_length; xx += 8)
            {
                const uint32_t bufidx{((yy + y) * width + (xx + x)) >> 3};
                const uint32_t tilidx{(yy * tile.side_length + xx) >> 3};
                const auto data{tile.data[tilidx]};

                const auto byte_x0{(data >> 2) & 0b0011'1111};
                const auto byte_x1{(data << 6) & 0b1100'0000};
                buffer[bufidx] |= byte_x0;
                buffer[bufidx + 1] |= byte_x1;
            }
        }
    }};
    auto &&mod3{[&]() {
        for (uint32_t yy = 0; yy < tile.side_length; ++yy)
        {
            for (uint32_t xx = 0; xx < tile.side_length; xx += 8)
            {
                const uint32_t bufidx{((yy + y) * width + (xx + x)) >> 3};
                const uint32_t tilidx{(yy * tile.side_length + xx) >> 3};
                const auto data{tile.data[tilidx]};

                const auto byte_x0{(data >> 3) & 0b0001'1111};
                const auto byte_x1{(data << 5) & 0b1110'0000};
                buffer[bufidx] |= byte_x0;
                buffer[bufidx + 1] |= byte_x1;
            }
        }
    }};
    auto &&mod4{[&]() {
        for (uint32_t yy = 0; yy < tile.side_length; ++yy)
        {
            for (uint32_t xx = 0; xx < tile.side_length; xx += 8)
            {
                const uint32_t bufidx{((yy + y) * width + (xx + x)) >> 3};
                const uint32_t tilidx{(yy * tile.side_length + xx) >> 3};
                const auto data{tile.data[tilidx]};

                const auto byte_x0{(data >> 4) & 0b0000'1111};
                const auto byte_x1{(data << 4) & 0b1111'0000};
                buffer[bufidx] |= byte_x0;
                buffer[bufidx + 1] |= byte_x1;
            }
        }
    }};
    auto &&mod5{[&]() {
        for (uint32_t yy = 0; yy < tile.side_length; ++yy)
        {
            for (uint32_t xx = 0; xx < tile.side_length; xx += 8)
            {
                const uint32_t bufidx{((yy + y) * width + (xx + x)) >> 3};
                const uint32_t tilidx{(yy * tile.side_length + xx) >> 3};
                const auto data{tile.data[tilidx]};

                const auto byte_x0{(data >> 5) & 0b0000'0111};
                const auto byte_x1{(data << 3) & 0b1111'1000};
                buffer[bufidx] |= byte_x0;
                buffer[bufidx + 1] |= byte_x1;
            }
        }
    }};
    auto &&mod6{[&]() {
        for (uint32_t yy = 0; yy < tile.side_length; ++yy)
        {
            for (uint32_t xx = 0; xx < tile.side_length; xx += 8)
            {
                const uint32_t bufidx{((yy + y) * width + (xx + x)) >> 3};
                const uint32_t tilidx{(yy * tile.side_length + xx) >> 3};
                const auto data{tile.data[tilidx]};

                const auto byte_x0{(data >> 6) & 0b0000'0011};
                const auto byte_x1{(data << 2) & 0b1111'1100};
                buffer[bufidx] |= byte_x0;
                buffer[bufidx + 1] |= byte_x1;
            }
        }
    }};
    auto &&mod7{[&]() {
        for (uint32_t yy = 0; yy < tile.side_length; ++yy)
        {
            for (uint32_t xx = 0; xx < tile.side_length; xx += 8)
            {
                const uint32_t bufidx{((yy + y) * width + (xx + x)) >> 3};
                const uint32_t tilidx{(yy * tile.side_length + xx) >> 3};
                const auto data{tile.data[tilidx]};

                const auto byte_x0{(data >> 7) & 0b0000'0001};
                const auto byte_x1{(data << 1) & 0b1111'1110};
                buffer[bufidx] |= byte_x0;
                buffer[bufidx + 1] |= byte_x1;
            }
        }
    }};

    const auto bit_offset{(y * width + x) & 0b111};

    switch (bit_offset)
    {
    case 0:
        mod0();
        break;
    case 1:
        mod1();
        break;
    case 2:
        mod2();
        break;
    case 3:
        mod3();
        break;
    case 4:
        mod4();
        break;
    case 5:
        mod5();
        break;
    case 6:
        mod6();
        break;
    case 7:
        mod7();
        break;
    }
}

void blit_2bpp(uint8_t *__restrict buffer, uint32_t width, uint32_t x, uint32_t y, Tile tile) noexcept
{
    /*
     * much like 4bpp, except there's 4 possible offset cases to handle
     *
     * 4 pixels per byte, so the x offset will make us:
     *
     *   x%4=0
     *   XXXXXXXX YYYYYYYY
     *   p0p1p2p3
     *
     *   x%4=1
     *   XXXXXXXX YYYYYYYY
     *   --p0p1p2 p3
     *
     *   x%4=2
     *   XXXXXXXX YYYYYYYY
     *   ----p0p1 p2p3
     *
     *   x%4=3
     *   XXXXXXXX YYYYYYYY
     *   ------p0 p1p2p3
     *
     */

    auto &&mod0{[&]() {
        for (uint32_t yy = 0; yy < tile.side_length; ++yy)
        {
            for (uint32_t xx = 0; xx < tile.side_length; xx += 4)
            {
                const uint32_t bufidx{(yy + y) * width + (xx + x)};
                const uint32_t tilidx{yy * tile.side_length + xx};
                buffer[bufidx >> 2] = tile.data[tilidx >> 2];
            }
        }
    }};
    auto &&mod1{[&]() {
        for (uint32_t yy = 0; yy < tile.side_length; ++yy)
        {
            for (uint32_t xx = 0; xx < tile.side_length; xx += 4)
            {
                const uint32_t bufidx{((yy + y) * width + (xx + x)) >> 2};
                const uint32_t tilidx{(yy * tile.side_length + xx) >> 2};
                const auto data{tile.data[tilidx]};

                const auto byte_x0{(data >> 2) & 0b0011'1111};
                const auto byte_x1{(data << 6) & 0b1100'0000};
                buffer[bufidx] |= byte_x0;
                buffer[bufidx + 1] |= byte_x1;
            }
        }
    }};
    auto &&mod2{[&]() {
        for (uint32_t yy = 0; yy < tile.side_length; ++yy)
        {
            for (uint32_t xx = 0; xx < tile.side_length; xx += 4)
            {
                const uint32_t bufidx{((yy + y) * width + (xx + x)) >> 2};
                const uint32_t tilidx{(yy * tile.side_length + xx) >> 2};
                const auto data{tile.data[tilidx]};

                const auto byte_x0{(data >> 4) & 0b0000'1111};
                const auto byte_x1{(data << 4) & 0b1111'0000};
                buffer[bufidx] |= byte_x0;
                buffer[bufidx + 1] |= byte_x1;
            }
        }
    }};
    auto &&mod3{[&]() {
        for (uint32_t yy = 0; yy < tile.side_length; ++yy)
        {
            for (uint32_t xx = 0; xx < tile.side_length; xx += 4)
            {
                const uint32_t bufidx{((yy + y) * width + (xx + x)) >> 2};
                const uint32_t tilidx{(yy * tile.side_length + xx) >> 2};
                const auto data{tile.data[tilidx]};

                const auto byte_x0{(data >> 6) & 0b0000'0011};
                const auto byte_x1{(data << 2) & 0b1111'1100};
                buffer[bufidx] |= byte_x0;
                buffer[bufidx + 1] |= byte_x1;
            }
        }
    }};

    const auto nibble_offset{(y * width + x) & 0b11};

    switch (nibble_offset)
    {
    case 0:
        mod0();
        break;
    case 1:
        mod1();
        break;
    case 2:
        mod2();
        break;
    case 3:
        mod3();
        break;
    }
}

/*
 _  _   _
| || | | |__  _ __  _ __
| || |_| '_ \| '_ \| '_ \
|__   _| |_) | |_) | |_) |
   |_| |_.__/| .__/| .__/
             |_|   |_|

*/
void blit_4bpp(uint8_t *__restrict buffer, uint32_t width, uint32_t x, uint32_t y, Tile tile) noexcept
{
    if (tile.transparent)
    {
        impl_blit_4bpp_with_transparency(buffer, width, x, y, tile);
        return;
    }

    impl_blit_4bpp(buffer, width, x, y, tile);
}

void impl_blit_4bpp(uint8_t *__restrict buffer, uint32_t width, uint32_t x, uint32_t y, Tile tile) noexcept
{
    /* lsn is pixel 0, msn is pixel 1, etc
     * this only applies to columns, not rows
     * so, if the index is even, we use the lsn
     * otherwise, the msn
     *
     * we always index through the tile in a consistent manner
     *
     * however, this isn't guarenteed for the video buffer,
     * due to the x, y offset requested.
     *
     * this leads to 2 possible states:
     *    #1 -> the nibbles to read-from and assign-to are the same
     *    #2 -> the nibbles are NOT the same
     * we are in state #1 if idx is even
     * we are in state #2 otherwise
     *
     * Also, I sort of forgot, if tile.side_length is NOT an even multiple of 2,
     * then we also need to handle the offset
     */
    const bool nibbles_match{0 == (x & 0b1)};
    const int rem{tile.side_length & 0b1};
    const int tile_pitch{tile.side_length + rem};
    if (nibbles_match)
    {
#ifdef PRINT_DEBUG
        std::cerr << "nibbles match\n";
#endif
        static_assert(sizeof(uint32_t) == 4);
        uint32_t yy = 0;
        uint32_t xx = 0;
        for (; yy < tile.side_length; ++yy)
        {
            for (; xx < tile.side_length - rem; xx += 2)
            {
                const uint32_t idx{(yy + y) * width + (xx + x)};
                const uint32_t idx2{yy * tile_pitch + xx};
                buffer[idx >> 1] = tile.data[idx2 >> 1];
            }
            if (rem)
            {
#ifdef PRINT_DEBUG
                std::cerr << "rem is " << rem << '\n';
#endif
                const uint32_t idx{(yy + y) * width + (xx + x + rem)};
                const uint32_t idx2{yy * tile_pitch + xx + rem};
                buffer[idx >> 1] &= 0b11110000;
                buffer[idx >> 1] |= (tile.data[idx2 >> 1] & 0b00001111);
            }
            xx = 0;
        }
    }
    else
    {
#ifdef PRINT_DEBUG
        std::cerr << "nibbles DONT match\n";
#endif
        uint32_t yy = 0;
        uint32_t xx = 0;
        for (; yy < tile.side_length; ++yy)
        {
#ifdef PRINT_DEBUG
            std::cerr << "row " << yy << '\n';
#endif
            for (; xx < tile.side_length - rem; xx += 2)
            {
#ifdef PRINT_DEBUG
                std::cerr << "col " << xx << '\n';
#endif
                const uint32_t idx{(yy + y) * width + (xx + x)};
                const uint32_t idx2{yy * tile_pitch + xx};
                const uint8_t lsn{static_cast<uint8_t>(tile.data[idx2 >> 1] & 0b1111U)};
                const uint8_t msn{static_cast<uint8_t>(((tile.data[idx2 >> 1]) >> 4) & 0b1111U)};
                buffer[idx >> 1] &= 0b00001111;
                buffer[idx >> 1] |= (lsn << 4);
                buffer[(idx >> 1) + 1] &= 0b11110000;
                buffer[(idx >> 1) + 1] |= msn;
            }
            if (rem)
            {
#ifdef PRINT_DEBUG
                std::cerr << "rem is " << rem << '\n';
                std::cerr << "col " << xx + rem << '\n';
#endif
                const uint32_t idx{(yy + y) * width + (xx + x)}; /* off by one error, which are rare for me... why?? */
                const uint32_t idx2{yy * tile_pitch + xx + rem};
#ifdef PRINT_DEBUG
                std::cerr << "vidbuf idx = " << idx << '\n';
                std::cerr << "tile   idx = " << idx2 << '\n';
#endif
                const uint8_t lsn{static_cast<uint8_t>(tile.data[idx2 >> 1] & 0b1111U)};
                buffer[idx >> 1] &= 0b00001111;
                buffer[idx >> 1] |= (lsn << 4);
            }
            xx = 0;
        }
    }
}

void impl_blit_4bpp_with_transparency(uint8_t *__restrict buffer, uint32_t width, uint32_t x, uint32_t y,
                                      Tile tile) noexcept
{
    const bool nibbles_match{0 ==
                             (x & 0b1)}; /* TODO this test and others like it restrict frame widths to an even number */
    const int rem{tile.side_length & 0b1};
    const int tile_pitch{tile.side_length + rem};
    if (nibbles_match)
    {
#ifdef PRINT_DEBUG
        std::cerr << "nibbles match\n";
#endif
        static_assert(sizeof(uint32_t) == 4);
        uint32_t yy = 0;
        uint32_t xx = 0;
        for (; yy < tile.side_length; ++yy)
        {
            for (; xx < tile.side_length - rem; xx += 2)
            {
                const uint32_t idx{align_byte((yy + y) * width + (xx + x), Format::GREY4)};
                const uint32_t idx2{align_byte(yy * tile_pitch + xx, Format::GREY4)};
                const auto tilebyte{tile.data[idx2]};
                auto &outbyte{buffer[idx]};
                switch ((((tilebyte & 0xF0) != 0) << 1) + ((tilebyte & 0x0F) != 0))
                {
                case 0: /* assign neither */
                    break;
                case 1: /* assign lsn */
                    outbyte = (outbyte & 0xF0) | (tilebyte & 0x0F);
                    break;
                case 2: /* assign msn */
                    outbyte = (outbyte & 0x0F) | (tilebyte & 0xF0);
                    break;
                case 3: /* assign both */
                    outbyte = tilebyte;
                    break;
                }
            }
            if (rem)
            {
                const uint32_t idx{align_byte((yy + y) * width + (xx + x + rem), Format::GREY4)};
                const uint32_t idx2{align_byte(yy * tile_pitch + xx + rem, Format::GREY4)};
                const auto tilebyte{tile.data[idx2] & 0b00001111};
                if (tilebyte)
                {
                    buffer[idx] &= 0b11110000;
                    buffer[idx] |= tilebyte;
                }
            }
            xx = 0;
        }
    }
    else
    {
#ifdef PRINT_DEBUG
        std::cerr << "nibbles dont match\n";
#endif
        uint32_t yy = 0;
        uint32_t xx = 0;
        for (; yy < tile.side_length; ++yy)
        {
            for (; xx < tile.side_length - rem; xx += 2)
            {
                const uint32_t idx{align_byte((yy + y) * width + (xx + x), Format::GREY4)};
                const uint32_t idx2{align_byte(yy * tile_pitch + xx, Format::GREY4)};
                const uint8_t lsn{static_cast<uint8_t>(tile.data[idx2] & 0b1111U)};
                const uint8_t msn{static_cast<uint8_t>(((tile.data[idx2]) >> 4) & 0b1111U)};

                switch (((msn != 0) << 1) + (lsn != 0))
                {
                case 0: /* assign neither */
                    break;
                case 1: /* assign lsn */
                    buffer[idx] &= 0b00001111;
                    buffer[idx] |= (lsn << 4);
                    break;
                case 2: /* assign msn */
                    buffer[idx + 1] &= 0b11110000;
                    buffer[idx + 1] |= msn;
                    break;
                case 3: /* assign both */
                    buffer[idx] &= 0b00001111;
                    buffer[idx] |= (lsn << 4);
                    buffer[idx + 1] &= 0b11110000;
                    buffer[idx + 1] |= msn;
                    break;
                }
            }
            if (rem)
            {
                const uint32_t idx{align_byte((yy + y) * width + (xx + x), Format::GREY4)};
                const uint32_t idx2{align_byte(yy * tile_pitch + xx + rem, Format::GREY4)};
                const uint8_t lsn{static_cast<uint8_t>(tile.data[idx2] & 0b1111U)};
                if (lsn)
                {
                    buffer[idx] &= 0b00001111;
                    buffer[idx] |= (lsn << 4);
                }
            }
            xx = 0;
        }
    }
}

void blit_8bpp(uint8_t *__restrict buffer, uint32_t width, uint32_t x, uint32_t y, Tile tile) noexcept
{
    if (tile.transparent)
    {
        impl_blit_8bpp_with_transparency(buffer, width, x, y, tile);
        return;
    }

    impl_blit_8bpp(buffer, width, x, y, tile);
}

void impl_blit_8bpp(uint8_t *__restrict buffer, uint32_t width, uint32_t x, uint32_t y, Tile tile) noexcept
{
    uint32_t iidx{y * width + x};
    for (uint32_t tidx = 0; tidx < tile.side_length * tile.side_length; tidx += tile.side_length)
    {
        std::memcpy(&buffer[iidx], &tile.data[tidx], tile.side_length);
        iidx += width;
    }
}

void impl_blit_8bpp_with_transparency(uint8_t *__restrict buffer, uint32_t width, uint32_t x, uint32_t y,
                                      Tile tile) noexcept
{
    uint32_t iidx{y * width + x};
    for (uint32_t rowidx = 0; rowidx < tile.side_length * tile.side_length; rowidx += tile.side_length)
    {
        for (uint32_t tidx = rowidx; tidx < tile.side_length + rowidx; ++tidx)
        {
            if (!tile.data[tidx])
            {
                continue;
            }
            buffer[iidx + tidx - rowidx] = tile.data[tidx];
        }
        iidx += width;
    }
}

void blit_16bpp(uint8_t *__restrict buffer, uint32_t width, uint32_t x, uint32_t y, Tile tile) noexcept
{
    uint32_t iidx{y * width + x};
    for (uint32_t tidx = 0; tidx < tile.side_length * tile.side_length; tidx += tile.side_length)
    {
        std::memcpy(&buffer[align_byte(iidx, Format::RGB565)], &tile.data[align_byte(tidx, Format::RGB565)],
                    tile.side_length * 2);
        iidx += width;
    }
}
} // namespace screen
