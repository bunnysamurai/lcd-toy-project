#if !defined(TILEDEF_H)
#define TILEDEF_H

#include <cstdint>

#include "screen_def.h"

namespace screen
{

struct Tile final
{
    uint8_t side_length;
    Format format;
    const uint8_t *data;
};

/* a pixel value of 0 is considered transparent */
struct TransparencyTile final
{
    uint8_t side_length;
    Format format;
    const uint8_t *data;
};

} // namespace screen

#endif
