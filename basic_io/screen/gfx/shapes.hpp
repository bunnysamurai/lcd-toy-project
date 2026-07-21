#if !defined(SCREEN_GFX_SHAPES_HPP)
#define SCREEN_GFX_SHAPES_HPP

#include "defs.hpp"
#include <cstdint>

namespace screen::gfx
{
/** @brief Draw a line on the screen.
 * @param p1 Starting point of the line.
 * @param p2 Ending point of the line.
 * @param value Color value.  Will be interpreted using the screen's current
 * format.
 * @param thickenss Border thickness.  A value of '0' is undefined.
 */
void draw_line(Point p1, Point p2, uint32_t value, uint16_t thickness) noexcept;

/** @brief Draw a rectangle on the screen
 * @param rect Rectangle definition.
 * @param value Color value.  Will be interpreted using the screen's current
 * format.
 * @param thickenss Border thickness.  A value of '0' means 'filled'.
 */
void draw_rect(Rect r, uint32_t value, uint16_t thickness) noexcept;

/** @brief Draw a circle on the screen
 * @param center of the circle
 * @param radius of the circle
 * @param value Color value.  Will be interpreted using the screen's current
 * format.
 * @param thickenss Border thickness.  A value of '0' means 'filled'.
 */
void draw_circle(Point center, uint16_t radius, uint32_t value, uint16_t thickness) noexcept;

/** @brief Draw a highlight border around a rectangle on the screen
 *  @param rect Square area on the screen to draw a border around
 *  @param thickenss of the border
 *  @param top_left_value Pixel value of the top and left borders.  Will be interpreted using the screen's current
 * format.
 *  @param bottm_right_value Pixel value of the bottom and left borders.  Will be interpreted using the screen's current
 * format.
 */
void draw_border(Rect rect, int thickness, uint32_t top_left_value, uint32_t bottom_right_value) noexcept;

} // namespace screen::gfx
#endif