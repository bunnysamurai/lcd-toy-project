#if !defined(SCREEN_HPP)
#define SCREEN_HPP

#include <cstdint>

#include "TileDef.h"
#include "screen_def.h"

namespace screen {

/* =====================================================================================
 */
[[nodiscard]] bool init(Position virtual_topleft, Dimensions virtual_size,
                        Format format) noexcept;

void init_clut(const Clut* entries, uint32_t length) noexcept;

[[nodiscard]] uint32_t get_buf_len();

[[nodiscard]] uint8_t *get_video_buffer() noexcept;
void set_video_buffer(const uint8_t *buffer) noexcept;

[[nodiscard]] Format get_format() noexcept;
void set_format(Format) noexcept;

[[nodiscard]] Dimensions get_virtual_screen_size() noexcept;
void set_virtual_screen_size(Position new_topleft,
                             Dimensions new_size) noexcept;

[[nodiscard]] Dimensions get_physical_screen_size() noexcept;

/* =====================================================================================
 */

/** @brief Get lateset touch report.
 *
 *  Check the timestamp, as these may be stale.
 *
 *  Once called, will emtpy the underlying ring buffer (it's only 1 element in
 * size).
 *
 * @param[out] out The latest report.  Don't use if return is false.
 *
 * @return True if a new report is availble, false otherwise.
 */
[[nodiscard]] bool get_touch_report(TouchReport &out);

/* =====================================================================================
 */

/** @brief Switch to video-buffer mode
 * The most basic interface, you can gain access to the raw video buffer.
 * It also has one bell, a convenience function to blit-in a Tile
 */
void clear_screen();
void draw_tile(uint32_t xpos, uint32_t ypos, Tile tile);
void fill_screen(uint32_t raw_value); /* does not take bpp into account, so be aware */

/** @brief Switch to text-only mode
 *
 * Think of this as a specialized version of video mode.
 */
[[nodiscard]] bool set_console_mode() noexcept;

/** @brief Sets screen to black.
 */
void clear_console();

/** @brief Console size, in characters
 *
 * @return The maximum number of characters (width) and lines (height) of the
 * console.
 */
[[nodiscard]] Dimensions get_console_width_and_height() noexcept;

void draw_letter(uint32_t column, uint32_t line, char c);

/** @brief Scroll the text on the screen
 *
 * @param lines + if scroll upwards, - if scroll downwards
 */
void scroll_up(int lines);

} // namespace screen
#endif