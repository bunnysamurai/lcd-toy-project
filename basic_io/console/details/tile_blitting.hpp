#if !defined(TILE_BLITTING_HPP)
#define TILE_BLITTING_HPP

#include <cstddef>
#include <cstdint>

#include "../TileDef.h"

namespace tile {

/** @brief blit in tile on a 1bpp buffer
 *
 * @param buffer Raw video buffer
 * @param width width of video frame, in pixels
 * @param x Column offset, in pixels, to blit in the tile
 * @param y Row offset, in pixels, to blit in the tile
 * @param tile The tile to blit
 */
void blit_1bpp(uint8_t *__restrict buffer, size_t width, size_t x, size_t y,
               Tile tile);

void blit_2bpp(uint8_t *__restrict buffer, size_t width, size_t x, size_t y,
               Tile tile);

void blit_4bpp(uint8_t *__restrict buffer, size_t width, size_t x, size_t y,
               Tile tile);

void blit_8bpp(uint8_t *__restrict buffer, size_t width, size_t x, size_t y,
               Tile tile);

void blit_16bpp(uint8_t *__restrict buffer, size_t width, size_t x, size_t y,
                Tile tile);

} // namespace tile

#endif