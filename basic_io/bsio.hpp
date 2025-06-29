#if !defined(BSIO_HPP)
#define BSIO_HPP

#include "console/TileDef.h"
#include "screen/screen_def.h"

namespace bsio {

bool init();

void clear_console();
void draw_tile(uint32_t xpos, uint32_t ypos, Tile tile);
uint8_t* get_video_buffer();

} // namespace bsio

#endif
