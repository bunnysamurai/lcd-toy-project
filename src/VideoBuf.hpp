#ifndef VIDEOBUF_HPP
#define VIDEOBUF_HPP

#include <array>
#include <cstddef>
#include <cstdint>

namespace
{

    /**
     * @param screen_width_in_pixels Number of pixels per row of the display.
     * @param bits_per_pixel Duh
     * @return Number of characters (or CharType) per row of the display.
     */
    template <class CharType>
    constexpr size_t to_character_width(const size_t screen_width_in_pixels, const size_t bits_per_pixel)
    {
        return screen_width_in_pixels * bits_per_pixel / CharType::pixel_width;
    }
    /**
     * @param screen_height_in_pixels Number of pixels per column of the display.
     * @param bits_per_pixel Duh
     * @return Number of characters (or CharType) per column of the display.
     */
    template <class CharType>
    constexpr size_t to_character_height(const size_t screen_height_in_pixels, const size_t bits_per_pixel)
    {
        return screen_height_in_pixels * bits_per_pixel / CharType::pixel_height;
    }
    constexpr size_t compute_video_buffer_length(size_t width, size_t height, size_t bits_per_pixel)
    {
        return width * height * bits_per_pixel / 8;
    }
}

template <size_t WIDTH_IN_PIXELS, size_t HEIGHT_IN_PIXELS, size_t BPP>
struct TileBuffer
{
public:
    using buffer_type = std::array<uint8_t, compute_video_buffer_length(WIDTH_IN_PIXELS, HEIGHT_IN_PIXELS, BPP)>;

    explicit TileBuffer(buffer_type &buf) : video_buf{buf} {}

    template <class TileT>
    [[nodiscard]] static constexpr size_t max_tiles_per_row()
    {
        return to_character_width<TileT>(WIDTH_IN_PIXELS, BPP);
    }
    template <class TileT>
    [[nodiscard]] static constexpr size_t max_tiles_per_column()
    {
        return to_character_height<TileT>(HEIGHT_IN_PIXELS, BPP);
    }

    // start with our customization point method of choice, plain ol' ADL!
    /**
     * @param video_buf Whatever the video buffer data structure is.  This will be the default implementation?
     * @param tile The tile to print
     * @param x Column, in characters, in native screen display orientation
     * @param y Row, in characters, in native screen display orientation
     */
    template <class TileT>
    friend constexpr void write_tile(TileBuffer &video_buf, const TileT &tile, uint32_t x, uint32_t y)
    {
        constexpr auto BITS_PER_BYTE{8};
        constexpr auto COLUMN_INCREMENT{TileT::pixel_width * BPP / BITS_PER_BYTE};
        constexpr auto TILE_ELEMENT_ROW_INCREMENT{video_buf.template max_tiles_per_row<TileT>()};
        constexpr auto ROW_INCREMENT{TILE_ELEMENT_ROW_INCREMENT * TileT::pixel_height};
        for (uint idx = y * ROW_INCREMENT + x * COLUMN_INCREMENT, ii = 0; ii < size(tile); idx += TILE_ELEMENT_ROW_INCREMENT, ++ii)
        {
            video_buf.video_buf[idx] = tile[ii];
        }
    }

private:
    buffer_type &video_buf;
};

#endif