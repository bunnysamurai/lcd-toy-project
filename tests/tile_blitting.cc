#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <iostream>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "screen/TileDef.h"
#include "screen/tile_blitting.hpp"

using screen::Format;
using screen::Tile;

class Blitting : public ::testing::Test
{
  protected:
    static constexpr uint32_t ROWS{10};
    static constexpr uint32_t COLS{10};

    auto create_video_buffer(const auto fill)
    {
        std::array<uint8_t, ROWS * COLS> video_buffer;
        std::ranges::fill(video_buffer, fill);
        return video_buffer;
    }

    void set_byte(std::array<uint8_t, ROWS * COLS> &buffer, uint32_t xpos, uint32_t ypos, uint8_t value)
    {
        buffer[ypos * COLS + xpos] = value;
    }

    void set_16bpp(std::array<uint8_t, ROWS * COLS> &buffer, uint32_t xpos, uint32_t ypos, uint16_t value)
    {
        const auto idx{ypos * COLS / 2 + xpos};
        buffer[idx * 2] = value & 0x00FF;
        buffer[idx * 2 + 1] = (value & 0xFF00) >> 8;
    }

    void set_4bpp(std::array<uint8_t, ROWS * COLS> &buffer, uint32_t xpos, uint32_t ypos, uint8_t value)
    {
        const auto idx{ypos * COLS + xpos};
        if (idx % 2 == 0)
        {
            buffer[idx >> 1] = (buffer[idx >> 1] & 0xF0) | value;
        }
        else
        {
            buffer[idx >> 1] = (buffer[idx >> 1] & 0x0F) | (value << 4);
        }
    }
};

namespace tests
{

static constexpr bool PRINT_DEBUG{true};

/* blit_4bpp */
[[nodiscard]] bool test_4bpp() noexcept
{

    bool status{true};

    /* ODD tile is 3x3 of alternating 1/0's per nibble */
    std::array<uint8_t, 6> odd_tile_data{0b10101101, 0b00000101, 0b10100101, 0b00000101, 0b10100101, 0b00001101};
    Tile odd_tile{.side_length = 3, .format = Format::RGB565_LUT4, .data = odd_tile_data.data()};
    /* EVEN tile is 4x4 of alternating 1/0's per nibble */
    std::array<uint8_t, 8> even_tile_data{0b10100111, 0b10100101, 0b10100101, 0b10100101,
                                          0b10100101, 0b10100101, 0b10100101, 0b10100111};
    Tile even_tile{.side_length = 4, .format = Format::RGB565_LUT4, .data = even_tile_data.data()};

    /* our video buffer is 6x6 pixels */
    auto &&create_video_buffer{[]() { return std::array<uint8_t, 6 * 6 / 2>{}; }};

    /* our test cases are:
     *  even tile at {x,y} 0,0
     *  even tile at {x,y} 1,0
     *  odd tile at {x,y} 0,0
     *  odd tile at {x,y} 1,0
     */
    auto &&test_apparatus{[&](const auto expected, const auto tile, const auto xpos, const auto ypos) {
        auto vidbuf{create_video_buffer()};
        blit_4bpp(vidbuf.data(), 6, xpos, ypos, tile);

        const bool result{std::equal(std::begin(expected), std::end(expected), std::begin(vidbuf), std::end(vidbuf))};

        if (!result && PRINT_DEBUG)
        {
            auto &&print_array{[](const auto arr, const auto msg) {
                std::cerr << msg << " { ";
                for (const auto c : arr)
                {
                    std::cerr << +c << ' ';
                }
                std::cerr << "}\n";
            }};

            std::cerr << "test_4bpp, " << xpos << ", " << ypos << ", sidelength = " << +tile.side_length << '\n';
            print_array(expected, "  expected");
            print_array(vidbuf, "  vidbuf  ");
        }

        return result;
    }};
    {
        /* even tile at {x,y} 0,0 */
        const auto expected1{[&]() {
            auto buf{create_video_buffer()};
            buf[0] = even_tile_data[0];
            buf[1] = even_tile_data[1];

            buf[3] = even_tile_data[2];
            buf[4] = even_tile_data[3];

            buf[6] = even_tile_data[4];
            buf[7] = even_tile_data[5];

            buf[9] = even_tile_data[6];
            buf[10] = even_tile_data[7];

            return buf;
        }()};
        status &= test_apparatus(expected1, even_tile, 0, 0);

        /* even tile at {x,y} 1,0 */
        const auto expected2{[&]() {
            /* EVEN tile is 4x4 of alternating 1/0's per nibble */
            auto buf{create_video_buffer()};
            // 0,0 of tile -> 1,0 of vid
            // 1,0 of tile -> 2,0 of vid
            // 2,0 of tile -> 3,0 of vid
            // 3,0 of tile -> 4,0 of vid
            buf[0] |= even_tile_data[0] << 4;
            buf[1] = even_tile_data[0] >> 4 | even_tile_data[1] << 4;
            buf[2] |= even_tile_data[1] >> 4;

            buf[3] |= even_tile_data[2] << 4;
            buf[4] = even_tile_data[2] >> 4 | even_tile_data[3] << 4;
            buf[5] |= even_tile_data[3] >> 4;

            buf[6] |= even_tile_data[4] << 4;
            buf[7] = even_tile_data[4] >> 4 | even_tile_data[5] << 4;
            buf[8] |= even_tile_data[5] >> 4;

            buf[9] |= even_tile_data[6] << 4;
            buf[10] = even_tile_data[6] >> 4 | even_tile_data[7] << 4;
            buf[11] |= even_tile_data[7] >> 4;

            return buf;
        }()};
        status &= test_apparatus(expected2, even_tile, 1, 0);

        /* odd tile at {x,y} 0,0 */
        const auto expected3{[&]() {
            /* ODD tile is 3x3 of alternating 1/0's per nibble */
            auto buf{create_video_buffer()};

            /* Tile layout in memory, note rows are byte-aligned
             *  0000 1111 2222 xxxx
             *  3333 4444 5555 xxxx
             *  6666 7777 8888 xxxx
             */
            buf[0] = odd_tile_data[0];
            buf[1] |= odd_tile_data[1] & 0b1111;

            buf[3] = odd_tile_data[2];
            buf[4] = odd_tile_data[3] & 0b1111;

            buf[6] = odd_tile_data[4];
            buf[7] = odd_tile_data[5] & 0b1111;

            return buf;
        }()};
        status &= test_apparatus(expected3, odd_tile, 0, 0);

        /* odd tile at {x,y} 1,0 */
        const auto expected4{[&]() {
            /* ODD tile is 3x3 of alternating 1/0's per nibble */
            auto buf{create_video_buffer()};

            /* Tile layout in memory, note rows are byte-aligned
             * nb/row  0    1    2    3    4
             *   0    ---- 0000 1111 2222 xxxx
             *   1    ---- 3333 4444 5555 xxxx
             *   2    ---- 6666 7777 8888 xxxx
             */
            buf[0] |= (odd_tile_data[0] & 0b1111) << 4;
            buf[1] |= (odd_tile_data[0] >> 4) & 0b1111;
            buf[1] |= (odd_tile_data[1] & 0b1111) << 4;

            buf[3] |= (odd_tile_data[2] & 0b1111) << 4;
            buf[4] |= (odd_tile_data[2] >> 4) & 0b1111;
            buf[4] |= (odd_tile_data[3] & 0b1111) << 4;

            buf[6] |= (odd_tile_data[4] & 0b1111) << 4;
            buf[7] |= (odd_tile_data[4] >> 4) & 0b1111;
            buf[7] |= (odd_tile_data[5] & 0b1111) << 4;
            return buf;
        }()};
        status &= test_apparatus(expected4, odd_tile, 1, 0);
    }

    return status;
};

} // namespace tests

TEST_F(Blitting, FourBitsPerPixel)
{
    /* Tile */
    EXPECT_TRUE(tests::test_4bpp());
}

TEST_F(Blitting, FourBitsPerPixel_Transparent)
{
    static constexpr auto FORMAT{screen::Format::RGB565_LUT4};

    /* When nibbles of tile align with nibbles of video buffer */
    {
        static constexpr uint32_t XTILEPOSITION{0};
        static constexpr uint32_t YTILEPOSITION{0};
        static constexpr uint8_t FILL{0xFF};
        auto buffer{create_video_buffer(FILL)};
        /* clang-format off */
        static constexpr std::array<uint8_t, 3 * 2> tile_data{
            0b0010'0001, 0b0000'0000, 
            0b0000'0001, 0b0000'0011,
            0b0010'0000, 0b0000'0011};
        static constexpr uint32_t tile_pitch{ 3 + 1 };
        /* clang-format on */
        const auto expected_buffer{[&, this]() {
            auto result{create_video_buffer(FILL)};
            for (uint32_t yy = 0; yy < 3; ++yy)
            {
                for (uint32_t xx = 0; xx < 3; ++xx)
                {
                    const auto idx{yy * tile_pitch + xx};
                    const auto byteidx{screen::align_byte(idx, FORMAT)};
                    if (idx % 2 == 0)
                    {
                        const auto pix{tile_data[byteidx] & 0x0F};
                        if (pix)
                        {
                            set_4bpp(result, xx + XTILEPOSITION, yy + YTILEPOSITION, pix);
                        }
                    }
                    else
                    {
                        const auto pix{static_cast<uint8_t>(tile_data[byteidx] & 0xF0) >> 4};
                        if (pix)
                        {
                            set_4bpp(result, xx + XTILEPOSITION, yy + YTILEPOSITION, pix);
                        }
                    }
                }
            }
            return result;
        }()};
        screen::TransparencyTile tile{.side_length = 3, .format = FORMAT, .data = std::data(tile_data)};
        screen::blit_4bpp(std::data(buffer), COLS, XTILEPOSITION, YTILEPOSITION, tile);

        EXPECT_THAT(buffer, testing::ContainerEq(expected_buffer));
    }

    /* When nibbles of tile do not align with nibbles of video buffer */
    {
        static constexpr uint32_t XTILEPOSITION{1};
        static constexpr uint32_t YTILEPOSITION{0};
        static constexpr uint8_t FILL{0xFF};
        auto buffer{create_video_buffer(FILL)};
        /* clang-format off */
        static constexpr std::array<uint8_t, 3 * 2> tile_data{
            0b0010'0001, 0b0000'0000, 
            0b0000'0001, 0b0000'0011,
            0b0010'0000, 0b0000'0011};
        static constexpr uint32_t tile_pitch{ 3 + 1 };
        /* clang-format on */
        const auto expected_buffer{[&, this]() {
            auto result{create_video_buffer(FILL)};
            for (uint32_t yy = 0; yy < 3; ++yy)
            {
                for (uint32_t xx = 0; xx < 3; ++xx)
                {
                    const auto idx{yy * tile_pitch + xx};
                    const auto byteidx{screen::align_byte(idx, FORMAT)};
                    if (idx % 2 == 0)
                    {
                        const auto pix{tile_data[byteidx] & 0x0F};
                        if (pix)
                        {
                            set_4bpp(result, xx + XTILEPOSITION, yy + YTILEPOSITION, pix);
                        }
                    }
                    else
                    {
                        const auto pix{static_cast<uint8_t>(tile_data[byteidx] & 0xF0) >> 4};
                        if (pix)
                        {
                            set_4bpp(result, xx + XTILEPOSITION, yy + YTILEPOSITION, pix);
                        }
                    }
                }
            }
            return result;
        }()};
        screen::TransparencyTile tile{.side_length = 3, .format = FORMAT, .data = std::data(tile_data)};
        screen::blit_4bpp(std::data(buffer), COLS, XTILEPOSITION, YTILEPOSITION, tile);

        EXPECT_THAT(buffer, testing::ContainerEq(expected_buffer));
    }
}

TEST_F(Blitting, EightBitsPerPixel)
{
    constexpr uint32_t XTILEPOSITION{1};
    constexpr uint32_t YTILEPOSITION{1};

    auto buffer{create_video_buffer(9)};

    /* clang-format off */
        const std::array<uint8_t, 3 * 3> tile_data{
            1, 2, 0, 
            1, 0, 3, 
            0, 2, 3};
    /* clang-format on */

    const auto expected_buffer{[&, this]() {
        auto result{create_video_buffer(9)};
        for (uint32_t yy = 0; yy < 3; ++yy)
        {
            for (uint32_t xx = 0; xx < 3; ++xx)
            {
                const auto idx{yy * 3 + xx};
                set_byte(result, xx + XTILEPOSITION, yy + YTILEPOSITION, tile_data[idx]);
            }
        }
        return result;
    }()};

    screen::Tile tile{.side_length = 3, .format = screen::Format::RGB565_LUT8, .data = std::data(tile_data)};
    screen::blit_8bpp(std::data(buffer), COLS, XTILEPOSITION, YTILEPOSITION, tile);

    EXPECT_THAT(buffer, testing::ContainerEq(expected_buffer));
}

TEST_F(Blitting, EightBitsPerPixel_Transparent)
{
    static constexpr uint32_t XTILEPOSITION{1};
    static constexpr uint32_t YTILEPOSITION{1};

    auto buffer{create_video_buffer(9)};

    /* clang-format off */
    const std::array<uint8_t, 3 * 3> tile_data{
        1, 2, 0, 
        1, 0, 3, 
        0, 2, 3};
    /* clang-format on */

    const auto expected_buffer{[&, this]() {
        auto result{create_video_buffer(9)};
        for (uint32_t yy = 0; yy < 3; ++yy)
        {
            for (uint32_t xx = 0; xx < 3; ++xx)
            {
                const auto idx{yy * 3 + xx};
                if (tile_data[idx])
                {
                    set_byte(result, xx + XTILEPOSITION, yy + YTILEPOSITION, tile_data[idx]);
                }
            }
        }
        return result;
    }()};

    screen::TransparencyTile tile{
        .side_length = 3, .format = screen::Format::RGB565_LUT8, .data = std::data(tile_data)};

    screen::blit_8bpp(std::data(buffer), COLS, XTILEPOSITION, YTILEPOSITION, tile);

    EXPECT_THAT(buffer, testing::ContainerEq(expected_buffer));
}

TEST_F(Blitting, SixteenBitsPerPixel)
{
    static constexpr uint32_t XTILEPOSITION{0};
    static constexpr uint32_t YTILEPOSITION{0};
    static constexpr uint8_t FILL{0xFF};

    auto buffer{create_video_buffer(FILL)};

    /* clang-format off */
    const std::array<uint16_t, 3 * 3> tile_U16data{
        0xBEEF, 0xDEAD, 0, 
        0xDEAD, 0, 0xBEEF, 
        0, 0xB00B, 0xBEEF};
    std::array<uint8_t, 3 * 3 * 2 > tile_data;
    std::memcpy(std::data(tile_data), std::data(tile_U16data), std::size(tile_data));
    /* clang-format on */

    const auto expected_buffer{[&, this]() {
        auto result{create_video_buffer(FILL)};
        for (uint32_t yy = 0; yy < 3; ++yy)
        {
            for (uint32_t xx = 0; xx < 3; ++xx)
            {
                const auto idx{yy * 3 + xx};
                set_16bpp(result, xx + XTILEPOSITION, yy + YTILEPOSITION, tile_U16data[idx]);
            }
        }
        return result;
    }()};

    screen::Tile tile{.side_length = 3, .format = screen::Format::RGB565, .data = std::data(tile_data)};
    screen::blit_16bpp(std::data(buffer), COLS / 2, XTILEPOSITION, YTILEPOSITION, tile);

    EXPECT_THAT(buffer, testing::ContainerEq(expected_buffer));
}