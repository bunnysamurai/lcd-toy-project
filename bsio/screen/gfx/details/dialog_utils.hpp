#ifndef SCREEN_GFX_DIALOG_UTILS_HPP
#define SCREEN_GFX_DIALOG_UTILS_HPP

#include "../defs.hpp"

namespace screen::gfx::details
{

/* takes shapes in terms of characters and returns shapes in terms of pixels on screen */
[[nodiscard]] Size to_size_in_pixels(Size in_characters) noexcept;

/**
    @brief Gets a screen-centered Point for a given rectangle size
 */
[[nodiscard]] Point compute_screen_centered_top_left_point(Size rectangle_width_height) noexcept;

/**
    @brief Gets a screen-centered Rect for a given size
 */
[[nodiscard]] Rect compute_screen_centered_rectangle(Size rectangle_width_height) noexcept;

} // namespace screen::gfx::details

#endif