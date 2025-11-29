#if !defined(SCREEN_HPP)
#define SCREEN_HPP

#include <cstdint>
#include <limits>

#include "TileDef.h"
#include "screen_def.h"

namespace screen {

/* =====================================================================================
 */
[[nodiscard]] bool init(Position virtual_topleft, Dimensions virtual_size,
                        Format format) noexcept;

void init_clut(const Clut *entries, uint32_t length) noexcept;

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

/** @brief Clear the screen to black
 *  For most screen pixel formats, what this value actually needs to be is
 * pretty clear. For any lookup-table ones (RGB656_LUT4, for example), this
 * choice is less clear. By convention, the first index into any lookup-table is
 * the "clear" color.
 *
 *  If this is too restrictive, consider using fill_screen instead.
 */
void clear_screen();

/** @brief Draw a tile to the video buffer
 */
void draw_tile(uint32_t xpos, uint32_t ypos, Tile tile);

/** @brief Change a pixel in memory, format-aware */
void poke(uint32_t xpos, uint32_t ypos, uint32_t value) noexcept;

/** @brief Read a pixel in memory, format-aware */
[[nodiscard]] uint32_t peek(uint32_t xpos, uint32_t ypos) noexcept;

/** @brief fill the video buffer DIRECTLY
 *  does not take the screen's format into account, so be aware
 */
void fill_screen(uint32_t raw_value);

/** @brief Quickly fill a bunch of continguous rows, or lines, on the screen.
 *  This is screen format aware.
 *  Column start and finish must be byte-aligned.
 */
void fillrows(uint32_t value, uint32_t row_start, uint32_t row_finish,
              uint32_t column_start = std::numeric_limits<uint32_t>::min(),
              uint32_t column_finish = std::numeric_limits<uint32_t>::max());

/** @brief copy one line of the frame to another
 *    Does the right thing, regardless of display pixel format
 *    Option to specify a cropped extent
 *  If either begin or end column is out of the image frame, will clamp
 * appropriately.
 * Note the following: column offsets into the row must be byte-aligned relative
 * to the screen format.
 */
void copyrow(const uint32_t dst, const uint32_t src,
             uint32_t column_start = std::numeric_limits<uint32_t>::min(),
             uint32_t column_finish = std::numeric_limits<uint32_t>::max());

/** @brief Melt the screen, DOOM style
 *
 *  This is a very simple implementation; when melting we only replace the
 * background with a constant color value.
 *
 * @param replacement_value Pixel value to make the background. Is screen format
 * aware.
 */
void melt(uint32_t replacement_value);

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

/** @brief Draw a character to the console.
 */
void draw_letter(uint32_t column, uint32_t line, char c);

/** @brief Scroll the text on the screen
 *
 * @param lines + if scroll upwards, - if scroll downwards
 */
void scroll_up(int lines);

} // namespace screen
#endif