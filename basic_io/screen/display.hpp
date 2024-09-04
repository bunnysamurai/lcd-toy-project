#if !defined(SCREEN_DISPLAY_H)
#define SCREEN_DISPLAY_H

#include <cstdint>

namespace screen
{
  bool init(const uint8_t *video_buf, uint8_t bpp);
  void set_video_buffer(const uint8_t *buffer);
}
#endif