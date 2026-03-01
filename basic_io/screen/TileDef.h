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

struct TransparencyTile final
{
    Tile obj;
    uint32_t transparency_value;
};

} // namespace screen

#endif
