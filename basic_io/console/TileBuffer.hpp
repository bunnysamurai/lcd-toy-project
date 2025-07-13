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
    // const uint32_t x_char{x * tile.side_length};
    // const uint32_t y_char{y * tile.side_length};
    const uint32_t x_char{x};
    const uint32_t y_char{y};
    if (bitsizeof(tile.format) == BPP) {
      switch (BPP) {
      case 1:
        tile::blit_1bpp(std::data(video_buf.video_buf), WIDTH_IN_PIXELS, x_char,
                        y_char, tile);
        break;
      case 2:
        tile::blit_2bpp(std::data(video_buf.video_buf), WIDTH_IN_PIXELS, x_char,
                        y_char, tile);
        break;
      case 4:
        tile::blit_4bpp(std::data(video_buf.video_buf), WIDTH_IN_PIXELS, x_char,
                        y_char, tile);
        break;
      case 8:
        tile::blit_8bpp(std::data(video_buf.video_buf), WIDTH_IN_PIXELS, x_char,
                        y_char, tile);
        break;
      case 16:
        tile::blit_16bpp(std::data(video_buf.video_buf), WIDTH_IN_PIXELS,
                         x_char, y_char, tile);
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