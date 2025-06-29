#if !defined(TILEDEF_H)
#define TILEDEF_H

#include <cstdint>

#include "screen/screen_def.h"

struct Tile {
  uint8_t side_length;
  screen::Format format;
  const uint8_t *data;
};

#endif
