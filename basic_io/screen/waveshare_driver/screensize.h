#if !defined(SCREENSIZE_H)
#define SCREENSIZE_H

namespace screen_impl {

#if defined(WAVESHARE_240P)
constexpr uint32_t PHYSICAL_WIDTH_PIXELS{240U};
constexpr uint32_t PHYSICAL_HEIGHT_PIXELS{320U};
#endif

} // namespace screen_impl
#endif