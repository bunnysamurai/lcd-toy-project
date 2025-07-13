#if !defined(BSIO_HPP)
#define BSIO_HPP

#include "console/TileDef.h"
#include "screen/screen.hpp"

namespace bsio {

bool init();

/** @brief Switch to video-buffer mode 
 * The most basic interface, you can gain access to the raw video buffer.
 * It also has one bell, a convenience function to blit-in a Tile
 */
void set_video_mode(screen::Format fmt);
void clear_screen();
void draw_tile(uint32_t xpos, uint32_t ypos, Tile tile);
uint8_t* get_video_buffer();

/** @brief Switch to text-only mode 
 *
 * Think of this as a specialized version of video mode.
 */
void set_console_mode();

/** @brief Sets screen to black. 
 */
void clear_console();

/** @brief Console size, in characters
 *
 * @return The maximum number of characters (width) and lines (height) of the console.
 */
[[nodiscard]] screen::Dimensions get_console_width_and_height() noexcept;
void draw_letter(uint32_t column, uint32_t line, char c);


} // namespace bsio

#endif
