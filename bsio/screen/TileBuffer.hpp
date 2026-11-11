#ifndef VIDEOBUF_HPP
#define VIDEOBUF_HPP

#include <array>
#include <cstddef>
#include <cstdint>

#include "TileDef.h"
#include "tile_blitting.hpp"

template <size_t WIDTH_IN_PIXELS, size_t HEIGHT_IN_PIXELS, size_t BPP, size_t BUFLEN> class TileBuffer final
{
  public:
    static_assert(WIDTH_IN_PIXELS * HEIGHT_IN_PIXELS * BPP <= BUFLEN * 8U,
                  "TileBuffer misconfiguration: video buffer too small");

    using buffer_type = std::array<uint8_t, BUFLEN>;

    constexpr explicit TileBuffer(buffer_type &buf) noexcept : video_buf{buf}
    {
    }

    /**
     * @param video_buf Whatever the video buffer data structure is.  This will be
     * the default implementation?
     *
     * This is hot-path stuff, for sure.
     * Keep an eye on possible optimizations.
     *
     * @param tile The tile to print
     * @param x Column, in pixels, in native screen display orientation
     * @param y Row, in pixels, in native screen display orientation
     */
    constexpr void draw(screen::Tile tile, uint32_t x, uint32_t y) noexcept
    {
        if (bitsizeof(tile.format) != BPP)
        {
            return; /* no diagnostic required */
        }

        if constexpr (BPP == 1)
        {
            screen::blit_1bpp(std::data(video_buf), WIDTH_IN_PIXELS, x, y, tile);
        }
        else if constexpr (BPP == 2)
        {
            screen::blit_2bpp(std::data(video_buf), WIDTH_IN_PIXELS, x, y, tile);
        }
        else if constexpr (BPP == 4)
        {
            screen::blit_4bpp(std::data(video_buf), WIDTH_IN_PIXELS, x, y, tile);
        }
        else if constexpr (BPP == 8)
        {
            screen::blit_8bpp(std::data(video_buf), WIDTH_IN_PIXELS, x, y, tile);
        }
        else if constexpr (BPP == 16)
        {
            screen::blit_16bpp(std::data(video_buf), WIDTH_IN_PIXELS, x, y, tile);
        }
    }

  private:
    buffer_type &video_buf;
};

#endif