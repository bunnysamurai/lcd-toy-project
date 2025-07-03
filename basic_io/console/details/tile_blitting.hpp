#if !defined(TILE_BLITTING_HPP)
#define TILE_BLITTING_HPP

#include <cstddef>
#include <cstdint>

#include "../TileDef.h"

namespace tile {

void blit_4bpp(uint8_t *__restrict buffer, size_t width, size_t x, size_t y,
               Tile tile);

void blit_8bpp(uint8_t *__restrict buffer, size_t width, size_t x, size_t y,
               Tile tile);

void blit_16bpp(uint8_t *__restrict buffer, size_t width, size_t x, size_t y,
                Tile tile);

} // namespace tile

#endif