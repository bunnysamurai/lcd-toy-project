#if !defined(SCREEN_GFX_SHAPES_HPP)
#define SCREEN_GFX_SHAPES_HPP

#include "defs.hpp"
#include <cstdint>

namespace screen::gfx {
/** @brief Draw a line on the screen.
 * @param p1 Starting point of the line.
 * @param p2 Ending point of the line.
 * @param value Color value.  Will be interpreted using the screen's current
 * format.
 * @param thickenss Border thickness.  A value of '0' is undefined.
 */
void draw_line(Point p1, Point p2, uint32_t value, uint32_t thickness) noexcept;

/** @brief Draw a rectangle on the screen
 * @param rect Rectangle definition.
 * @param value Color value.  Will be interpreted using the screen's current
 * format.
 * @param thickenss Border thickness.  A value of '0' means 'filled'.
 */
void draw_rect(Rect r, uint32_t value, uint32_t thickness) noexcept;

/** @brief Draw a circle on the screen
 * @param center of the circle
 * @param radius of the circle
 * @param value Color value.  Will be interpreted using the screen's current
 * format.
 * @param thickenss Border thickness.  A value of '0' means 'filled'.
 */
void draw_circle(Point center, uint32_t radius, uint32_t value,
                 uint32_t thickness) noexcept;
} // namespace screen::gfx
#endif