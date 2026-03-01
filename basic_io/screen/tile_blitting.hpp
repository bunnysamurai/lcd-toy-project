#if !defined(TILE_BLITTING_HPP)
#define TILE_BLITTING_HPP

#include <cstddef>
#include <cstdint>

#include "TileDef.h"

namespace screen
{

/** @brief blit in 1bpp tile on a 1bpp buffer
 *
 * Tile rows must be byte aligned, so pad accordingly.
 *
 * @param buffer Raw video buffer
 * @param width width of video frame, in pixels
 * @param x Column offset, in pixels, to blit in the tile
 * @param y Row offset, in pixels, to blit in the tile
 * @param tile The tile to blit
 */
void blit_1bpp(uint8_t *__restrict buffer, uint32_t width, uint32_t x, uint32_t y, Tile tile) noexcept;

/** @brief blit in a 2bpp tile on a 2bpp buffer
 *
 * Tile rows must be byte aligned, so pad accordingly.
 *
 * @param buffer Raw video buffer
 * @param width width of video frame, in pixels
 * @param x Column offset, in pixels, to blit in the tile
 * @param y Row offset, in pixels, to blit in the tile
 * @param tile The tile to blit
 */
void blit_2bpp(uint8_t *__restrict buffer, uint32_t width, uint32_t x, uint32_t y, Tile tile) noexcept;

/** @brief blit in a 4bpp tile on a 4bpp buffer
 *
 * Tile rows must be byte aligned, so pad accordingly.
 *
 * @param buffer Raw video buffer
 * @param width width of video frame, in pixels
 * @param x Column offset, in pixels, to blit in the tile
 * @param y Row offset, in pixels, to blit in the tile
 * @param tile The tile to blit
 */
void blit_4bpp(uint8_t *__restrict buffer, uint32_t width, uint32_t x, uint32_t y, Tile tile) noexcept;

void blit_4bpp(uint8_t *__restrict buffer, uint32_t width, uint32_t x, uint32_t y, TransparencyTile tile) noexcept;

/** @brief blit in a 8bpp tile on a 8bpp buffer
 *
 * Tile rows must be byte aligned, so pad accordingly.
 *
 * @param buffer Raw video buffer
 * @param width width of video frame, in pixels
 * @param x Column offset, in pixels, to blit in the tile
 * @param y Row offset, in pixels, to blit in the tile
 * @param tile The tile to blit
 */
void blit_8bpp(uint8_t *__restrict buffer, uint32_t width, uint32_t x, uint32_t y, Tile tile) noexcept;

void blit_8bpp(uint8_t *__restrict buffer, uint32_t width, uint32_t x, uint32_t y, TransparencyTile tile) noexcept;

/** @brief blit in a 16bpp tile on a 16bpp buffer
 *
 * Tile rows must be byte aligned, so pad accordingly.
 *
 * @param buffer Raw video buffer
 * @param width width of video frame, in pixels
 * @param x Column offset, in pixels, to blit in the tile
 * @param y Row offset, in pixels, to blit in the tile
 * @param tile The tile to blit
 */
void blit_16bpp(uint8_t *__restrict buffer, uint32_t width, uint32_t x, uint32_t y, Tile tile) noexcept;

} // namespace screen

#endif