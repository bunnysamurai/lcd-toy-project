#if !defined(TILEDEF_H)
#define TILEDEF_H

#include <cstdint>

#include "screen_def.h"

namespace screen {

struct Tile {
  uint8_t side_length;
  Format format;
  const uint8_t *data;
};

} // namespace screen

#endif
