#ifndef VIDEOBUF_HPP
#define VIDEOBUF_HPP

#include <array>
#include <cstddef>
#include <cstdint>

#include "TileDef.h"
#include "details/tile_blitting.hpp"

namespace {

/**
 * @param screen_width_in_pixels Number of pixels per row of the display.
 * @param bits_per_pixel Duh
 * @return Number of characters (or CharType) per row of the display.
 */
template <class CharType>
constexpr size_t to_character_width(const size_t screen_width_in_pixels,
                                    const size_t bits_per_pixel) {
  return screen_width_in_pixels * bits_per_pixel / CharType::width_pixels;
}
/**
 * @param screen_height_in_pixels Number of pixels per column of the display.
 * @param bits_per_pixel Duh
 * @return Number of characters (or CharType) per column of the display.
 */
template <class CharType>
constexpr size_t to_character_height(const size_t screen_height_in_pixels,
                                     const size_t bits_per_pixel) {
  return screen_height_in_pixels * bits_per_pixel / CharType::height_pixels;
}
constexpr size_t compute_video_buffer_length(size_t width, size_t height,
                                             size_t bits_per_pixel) {
  return width * height * bits_per_pixel / 8;
}
} // namespace

#if 0 // TODO not yet
template <class font_type, class tiled_buffer_device>
class CharacterBuffer
{
public:
    /**
     * @brief Please draw this character at this row and column on the screen
     */
    friend constexpr void draw(CharacterBuffer &video_buf, char c, uint32_t column, uint32_t row)
    {
        constexpr auto BITS_PER_BYTE{8};
        constexpr auto COLUMN_INCREMENT{font_type::pixel_width * BPP / BITS_PER_BYTE};
        constexpr auto TILE_ELEMENT_ROW_INCREMENT{video_buf.template max_tiles_per_row<font_type>()};
        constexpr auto ROW_INCREMENT{TILE_ELEMENT_ROW_INCREMENT * font_type::pixel_height};

        const auto tile{decode(c)};
        for (uint idx = y * ROW_INCREMENT + x * COLUMN_INCREMENT, ii = 0; ii < size(tile); idx += TILE_ELEMENT_ROW_INCREMENT, ++ii)
        {
            video_buf.video_buf[idx] = tile[ii];
        }
    }

private:

    font_type decode(char c)
    {
        return decoder(c);
    }

    tiled_buffer_device video_buf;
};
#endif

#if 0 // not yet
/*
    Here we'll impose some conditions on the format of the TileT:
    It must specify it's native BPP storage
    If it's BPP is less than 8, then it's pixel width must be an even multiple of 8 (i.e. is byte aligned)
    Regardless of it's BPP, the underlying data must be stored as an array of bytes.

    The TileWriter will be interacting with a buffer that specifies it's display-mode BPP
    We'll handle any conversions necessary on the TileT's behalf?
*/
template <size_t WIDTH_IN_PIXELS, size_t HEIGHT_IN_PIXELS, size_t BPP, typename buffer_type>
class TileWriter
{
public:
    static_assert(compute_video_buffer_length(WIDTH_IN_PIXELS, HEIGHT_IN_PIXELS, BPP) <= std::tuple_size_v<buffer_type>);

    explicit TileWriter(buffer_type &buf) : video_buf{buf} {}

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
     * @brief blit the tile at pixel location (x,y)
     * @param video_buf Whatever the video buffer data structure is.  This will be the default implementation?
     * @param tile The tile to print
     * @param x Column, in pixels, in native screen display orientation
     * @param y Row, in pixels, in native screen display orientation
     */
    template <class TileT>
    friend constexpr void draw(TileWriter &video_buf, const TileT &tile, uint32_t x, uint32_t y)
    {
        constexpr auto BITS_PER_BYTE{sizeof(uint8_t) * 8};
        constexpr auto WIDTH_INC{std::max(1U, BITS_PER_BYTE / BPP)}; // This will depend on BPP of the display.  We always write a byte at a time.

        const auto HEIGHT_STOP{std::min(HEIGHT_IN_PIXELS, y + TileT::height_pixels)};
        const auto WIDTH_STOP{std::min(WIDTH_IN_PIXELS, x + TileT::width_pixels)};
        // we'll allow wrap-around for now

        for (uint yy = y; yy < HEIGHT_STOP; ++yy)
        {
            const auto yy_start{yy * WIDTH_IN_PIXELS};
            uint xx = x;
            const auto idx{yy_start + xx};
            if constexpr (BPP == 1)
            {
                /*
                value to blit = 1111'1111       <- this can be arbitrarily length, but constrained to integral multiples of 8??

                  1111'1111
                | 0000'0000  0000'0000
                -----------------------
                  1111'1111  0000'0000

                   1111'111  1
                | 0000'0000  0000'0000
                -----------------------
                  0111'1111  1000'0000

                    1111'11  11
                | 0000'0000  0000'0000
                -----------------------
                  0011'1111  1100'0000

                etc.

                          1  111'1111
                | 0000'0000  0000'0000
                -----------------------
                  0000'0001  1111'1110

                More generally, there can be more bits in between.  In which case, it's a matter of determining the
                intial bit shift, then iterating through all of the bytes on this row from the tile, shifting them as
                appropriate.
                */
                // the shift is rem
                const auto rem{xx & 0b0000'0111U};
                uint8_t rem_byte{}; // this receives the bits that were shifted out of the byte
            }
            else if (BPP == TileT::BPP)
            {
                video_buf.video_buf[idx] = tile[tile_idx]
            }
        }

        // constexpr auto COLUMN_INCREMENT{TileT::width_pixels * BPP / BITS_PER_BYTE};
        // constexpr auto TILE_ELEMENT_ROW_INCREMENT{video_buf.template max_tiles_per_row<TileT>()};
        // constexpr auto ROW_INCREMENT{TILE_ELEMENT_ROW_INCREMENT * TileT::pixel_height};
        // for (uint idx = y * ROW_INCREMENT + x * COLUMN_INCREMENT, ii = 0; ii < size(tile); idx += TILE_ELEMENT_ROW_INCREMENT, ++ii)
        // {
        //     video_buf.video_buf[idx] = tile[ii];
        // }
    }

    friend constexpr void clear(TileWriter &video_buf)
    {
        for (uint idx = 0; idx < size(video_buf.video_buf); ++idx)
        {
            video_buf.video_buf[idx] = uint8_t{255}; // TODO really need to abstract what is "white" and "black" for the display
        }
    }

    friend constexpr void scroll_left(TileWriter &video_buf, size_t count)
    {
        constexpr auto width{WIDTH_IN_PIXELS * BPP / 8};
        const auto lookahead{count};
        for (uint rowidx = 0; rowidx < size(video_buf.video_buf); rowidx += width)
        {
            for (uint idx = lookahead; idx < width; ++idx)
            {
                video_buf.video_buf[idx - lookahead + rowidx] = video_buf.video_buf[idx + rowidx];
            }
            for (uint idx = width - lookahead; idx < width; ++idx)
            {
                video_buf.video_buf[idx + rowidx] = 255; // TODO really need to abstract what is "white" and "black" for the display
            }
        }
    }

    friend constexpr void scroll_up(TileWriter &video_buf, size_t count)
    {
        constexpr auto width{WIDTH_IN_PIXELS * BPP / 8};
        const auto lookahead{width * count};
        for (uint idx = lookahead; idx < size(video_buf.video_buf); ++idx)
        {
            video_buf.video_buf[idx - lookahead] = video_buf.video_buf[idx];
        }
        for (uint idx = size(video_buf.video_buf) - lookahead; idx < size(video_buf.video_buf); ++idx)
        {
            video_buf.video_buf[idx] = 255; // TODO really need to abstract what is "white" and "black" for the display
        }
    }

private:
    buffer_type &video_buf;
};
#endif

template <size_t WIDTH_IN_PIXELS, size_t HEIGHT_IN_PIXELS, size_t BPP,
          size_t BUFLEN>
class TileBuffer {
public:
  static_assert(WIDTH_IN_PIXELS * HEIGHT_IN_PIXELS * BPP <= BUFLEN * 8U,
                "TileBuffer misconfiguration: video buffer too small");

  using buffer_type = std::array<uint8_t, BUFLEN>;

  explicit TileBuffer(buffer_type &buf) : video_buf{buf} {}

  template <class TileT>
  [[nodiscard]] static constexpr size_t max_tiles_per_row() {
    return to_character_width<TileT>(WIDTH_IN_PIXELS, BPP);
  }
  template <class TileT>
  [[nodiscard]] static constexpr size_t max_tiles_per_column() {
    return to_character_height<TileT>(HEIGHT_IN_PIXELS, BPP);
  }

  // start with our customization point method of choice, plain ol' ADL!
  /**
   * @param video_buf Whatever the video buffer data structure is.  This will be
   * the default implementation?
   * @param tile The tile to print
   * @param x Column, in characters, in native screen display orientation
   * @param y Row, in characters, in native screen display orientation
   */
  template <class TileT>
  friend constexpr void draw(TileBuffer &video_buf, const TileT &tile,
                             uint32_t x, uint32_t y) {
    constexpr auto BITS_PER_BYTE{8};
    constexpr auto COLUMN_INCREMENT{TileT::width_pixels * BPP / BITS_PER_BYTE};
    constexpr auto TILE_ELEMENT_ROW_INCREMENT{
        video_buf.template max_tiles_per_row<TileT>()};
    constexpr auto ROW_INCREMENT{TILE_ELEMENT_ROW_INCREMENT *
                                 TileT::height_pixels};
    for (uint32_t idx = y * ROW_INCREMENT + x * COLUMN_INCREMENT, ii = 0;
         ii < size(tile); idx += TILE_ELEMENT_ROW_INCREMENT, ++ii) {
      video_buf.video_buf[idx] = tile[ii];
    }
  }
  /**
   * @param video_buf Whatever the video buffer data structure is.  This will be
   * the default implementation?
   *
   * This is hot-path stuff, for sure.
   * Keep an eye on possible optimizations.
   *
   * @param tile The tile to print
   * @param x Column, in characters, in native screen display orientation
   * @param y Row, in characters, in native screen display orientation
   */
  friend constexpr void draw(TileBuffer &video_buf, Tile tile, uint32_t x,
                             uint32_t y) {
    if (bitsizeof(tile.format) == BPP) {
      switch (BPP) {
      // case 1:
      //   tile::blit_1bpp();
      // case 2:
      //   tile::blit_2bpp();
      case 4:
        tile::blit_4bpp(std::data(video_buf.video_buf), WIDTH_IN_PIXELS, x, y,
                        tile);
      case 8:
        tile::blit_8bpp(std::data(video_buf.video_buf), WIDTH_IN_PIXELS, x, y,
                        tile);
      case 16:
        tile::blit_16bpp(std::data(video_buf.video_buf), WIDTH_IN_PIXELS, x, y,
                         tile);
        break;
      default:
        break;
      }
    }
  }

  friend constexpr void clear(TileBuffer &video_buf) {
    for (uint32_t idx = 0; idx < size(video_buf.video_buf); ++idx) {
      video_buf.video_buf[idx] =
          uint8_t{255}; // TODO really need to abstract what is "white" and
                        // "black" for the display
    }
  }

  friend constexpr void scroll_left(TileBuffer &video_buf, size_t count) {
    constexpr auto width{WIDTH_IN_PIXELS * BPP / 8};
    const auto lookahead{count};
    for (uint32_t rowidx = 0; rowidx < size(video_buf.video_buf);
         rowidx += width) {
      for (uint32_t idx = lookahead; idx < width; ++idx) {
        video_buf.video_buf[idx - lookahead + rowidx] =
            video_buf.video_buf[idx + rowidx];
      }
      for (uint32_t idx = width - lookahead; idx < width; ++idx) {
        video_buf.video_buf[idx + rowidx] =
            255; // TODO really need to abstract what is "white" and "black" for
                 // the display
      }
    }
  }

  friend constexpr void scroll_up(TileBuffer &video_buf, size_t count) {
    constexpr auto width{WIDTH_IN_PIXELS * BPP / 8};
    const auto lookahead{width * count};
    for (uint32_t idx = lookahead; idx < size(video_buf.video_buf); ++idx) {
      video_buf.video_buf[idx - lookahead] = video_buf.video_buf[idx];
    }
    for (uint32_t idx = size(video_buf.video_buf) - lookahead;
         idx < size(video_buf.video_buf); ++idx) {
      video_buf.video_buf[idx] = 255; // TODO really need to abstract what is
                                      // "white" and "black" for the display
    }
  }

private:
  buffer_type &video_buf;
};

#endif