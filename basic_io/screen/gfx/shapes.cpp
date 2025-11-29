#include "shapes.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>

#include "../screen.hpp"

namespace screen::gfx {

namespace details {
/**
 Preconditions: xstart < xstop
                xstart and xstop are within image bounds
 */
static inline void handle_horizontal_line_case(uint32_t xstart, uint32_t xstop,
                                               uint32_t ypos,
                                               uint32_t value) noexcept {
  /* we might be misaligned, i.e. not on a byte boundary
   * in which case, we can
   *    check for misalignment, both at the start and the end
   *    apply the update to the misaligned byte at the start
   *    memset the middle
   *    apply the update to the misaligned byte at the end
   */

  const auto fmt{screen::get_format()};
  const auto dim{screen::get_virtual_screen_size()};
  const auto rowstart{ypos * dim.width};

  auto *p_buf{screen::get_video_buffer()};

  /* test for, and handle, byte-aligned formats */
  if (fmt == screen::Format::RGB565) {
    for (int idx = xstart + rowstart; idx < xstop + rowstart; idx += 2) {
      const auto linidx{screen::align_byte(idx, screen::Format::RGB565)};
      p_buf[linidx] = value & 0xFFU;
      p_buf[linidx + 1] = (value >> 8) & 0xFFU;
    }
    return;
  }
  if (fmt == screen::Format::RGB565_LUT8) {
    memset(&p_buf[xstart], value, xstop - xstart);
    return;
  }

  /* OTHERWISE, handle the non-byte aligned formats */
  const auto byte_value{screen::expand(value, fmt)};
  if (screen::subbyte_index(xstart + rowstart, fmt) != 0) {
    screen::poke(xstart, ypos, value);
    ++xstart;
  }
  xstart = screen::align_byte(xstart + rowstart, fmt);

  if (screen::subbyte_index(xstop + rowstart, fmt) != 0) {
    screen::poke(xstop - 1, ypos, value);
    --xstop;
  }
  xstop = screen::align_byte(xstop + rowstart, fmt);

  if (xstart < xstop) {
    memset(&p_buf[xstart], byte_value, xstop - xstart);
  }
}

static inline void handle_vertical_line_case(uint32_t ystart, uint32_t ystop,
                                             uint32_t xpos,
                                             uint32_t value) noexcept {
  while (ystart != ystop) {
    screen::poke(xpos, ystart++, value);
  }
}

} // namespace details

/** @brief Draw a line on the screen.
 * @param p1 Starting point of the line.
 * @param p2 Ending point of the line.
 * @param value Color value.  Will be interpreted using the screen's current
 * format.
 * @param thickenss Border thickness.  A value of '0' is undefined.
 */
void draw_line(Point p1, Point p2, uint32_t value,
               uint32_t thickness) noexcept {

  const auto dim{screen::get_virtual_screen_size()};

  /* Check the easy cases, first...
   *    if y position is the same, use memset (after checking alignment)
   *    if x position is the same, no memset, but iterate through the rows
   *    if x and y are not the same, use Bresenham */
  if (p1.y == p2.y) {
    auto ystart{p1.y - (thickness / 2)};
    for (uint32_t yy = ystart; yy < ystart + thickness; ++yy) {
      if (yy < dim.height) {
        const auto minx{std::min(p1.x, p2.x)};
        const auto maxx{std::max(p1.x, p2.x)};
        details::handle_horizontal_line_case(minx, maxx, yy, value);
      }
    }
    return;
  }

  if (p1.x == p2.x) {
    auto xstart{p1.x - (thickness / 2)};
    for (uint32_t xx = xstart; xx < xstart + thickness; ++xx) {
      if (xx < dim.width) {
        const auto miny{std::min(p1.y, p2.y)};
        const auto maxy{std::max(p1.y, p2.y)};
        details::handle_vertical_line_case(miny, maxy, xx, value);
      }
    }
    return;
  }

  /* not the easy cases?  Then Bresenham's it is!

    need to check which axis is minor(changes slower) and which is major(changes
    faster)
      most examples have the Y axis as minor and the X axis as major

    need to check if the minor axis needs to increase or decrease
      most examples have the minor axis increasing
      *
   */
  const bool y_is_minor{std::abs(static_cast<int>(p1.y - p2.y)) <
                        std::abs(static_cast<int>(p1.x - p2.x))};

  if (y_is_minor) {
    const auto startx{std::min(p1.x, p2.x)};
    const auto stopx{std::max(p1.x, p2.x)};
    const auto starty{static_cast<int16_t>(p1.x < p2.x ? p1.y : p2.y)};
    const auto stopy{static_cast<int16_t>(p1.x < p2.x ? p2.y : p1.y)};

    int16_t deltax{stopx - startx};
    int16_t deltay{stopy - starty};

    int16_t yi = 1;
    if (deltay < 0) {
      yi = -1;
      deltay = -deltay;
    }

    int16_t error{static_cast<int16_t>((deltay << 1) - deltax)};
    int16_t yy{starty};
    for (uint16_t xx = startx; xx < stopx; ++xx) {
      screen::poke(xx, yy, value);
      if (error > 0) {
        yy += yi;
        error = error + (2 * (deltay - deltax));
      } else {
        error = error + 2 * deltay;
      }
    }
  } else {
    /* X is the minor axis */
    const auto starty{std::min(p1.y, p2.y)};
    const auto stopy{std::max(p1.y, p2.y)};
    const auto startx{static_cast<int16_t>(p1.y < p2.y ? p1.x : p2.x)};
    const auto stopx{static_cast<int16_t>(p1.y < p2.y ? p2.x : p1.x)};

    int16_t deltax{stopx - startx};
    int16_t deltay{stopy - starty};

    int16_t xi = 1;
    if (deltax < 0) {
      xi = -1;
      deltax = -deltax;
    }

    int16_t error{static_cast<int16_t>((deltax << 1) - deltay)};
    int16_t xx{startx};
    for (uint16_t yy = starty; yy < stopy; ++yy) {
      screen::poke(xx, yy, value);
      if (error > 0) {
        xx += xi;
        error += ((deltax - deltay) << 1);
      } else {
        error += deltax << 1;
      }
    }
  }
}

/** @brief Draw a rectangle on the screen
 * @param rect Rectangle definition.
 * @param value Color value.  Will be interpreted using the screen's current
 * format.
 * @param thickenss Border thickness.  A value of '0' means 'filled'.
 */
void draw_rect(Rect r, uint32_t value, uint32_t thickness) noexcept {
  if (thickness == 0) {
    screen::fillrows(value, r.topleft.y, r.size.height, r.topleft.x,
                     r.size.width);
    return;
  }

  const Point topleft{r.topleft};
  const Point botleft{.x = r.topleft.x, .y = r.topleft.y + r.size.height - 1U};
  const Point topright{.x = r.topleft.x + r.size.width - 1U, .y = r.topleft.y};
  const Point botright{.x = r.topleft.x + r.size.width - 1U,
                       .y = r.topleft.y + r.size.height - 1U};

  draw_line(topleft, topright, value, thickness);
  draw_line(topleft, botleft, value, thickness);
  draw_line(botright, topright, value, thickness);
  draw_line(botright, botleft, value, thickness);
}

/** @brief Draw a circle on the screen
 * @param center of the circle
 * @param radius of the circle
 * @param value Color value.  Will be interpreted using the screen's current
 * format.
 * @param thickenss Border thickness.  A value of '0' means 'filled'.
 */
void draw_circle(Point center, uint32_t radius, uint32_t value,
                 uint32_t thickness) noexcept {}
} // namespace screen::gfx