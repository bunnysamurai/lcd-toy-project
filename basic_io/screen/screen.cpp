#include "screen.hpp"

#if defined(WAVESHARE_240P)
#include "waveshare_driver/screen_impl.hpp"
#endif

namespace screen {

void set_format(Format fmt) noexcept { screen_impl::set_format(fmt); }

Format get_format() noexcept { return screen_impl::get_format(); }

Dimensions get_virtual_screen_size() noexcept {
  return screen_impl::get_virtual_screen_size();
}

void set_virtual_screen_size([[maybe_unused]] Position new_topleft,
                             Dimensions new_size) noexcept {
  screen_impl::set_virtual_screen_size(new_topleft, new_size);
}

const uint8_t *get_video_buffer() noexcept {
  return screen_impl::get_video_buffer();
}

bool init(const uint8_t *video_buf, [[maybe_unused]] Position virtual_topleft,
          Dimensions virtual_size, Format format) noexcept {

  return screen_impl::init(video_buf, virtual_topleft, virtual_size, format);
}

void set_video_buffer(const uint8_t *buffer) noexcept {
  screen_impl::set_video_buffer(buffer);
}

[[nodiscard]] bool get_touch_report(TouchReport &out) {
  return screen_impl::get_touch_report(out);
}

} // namespace screen
