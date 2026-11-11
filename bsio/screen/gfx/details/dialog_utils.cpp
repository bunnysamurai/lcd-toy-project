#include "dialog_utils.hpp"

#include "../../screen.hpp"

#include "../../glyphs/letters.hpp"

namespace screen::gfx::details
{

namespace
{
constexpr auto GRID_WIDTH{glyphs::tile::width()};
constexpr auto GRID_HEIGHT{glyphs::tile::height()};
} // namespace

Size to_size_in_pixels(Size in_characters) noexcept
{

    /* based on the longest string, we need to add a buffer around it */
    const auto width_in_chars{in_characters.width};
    const auto height_in_chars{in_characters.height};

    return {.width = width_in_chars * GRID_WIDTH, .height = height_in_chars * GRID_HEIGHT};
}

Point compute_screen_centered_top_left_point(Size rectangle_width_height) noexcept
{
    const auto [swidth, sheight]{screen::get_virtual_screen_size()};
    const auto left_col{(swidth / 2) - ((rectangle_width_height.width / 2))};
    const auto top_row{(sheight / 2) - ((rectangle_width_height.height / 2))};

    return {.x = left_col, .y = top_row};
}

Rect compute_screen_centered_rectangle(Size rectangle_width_height) noexcept
{
    const auto topleft{compute_screen_centered_top_left_point(rectangle_width_height)};

    return {.topleft = topleft, .size = rectangle_width_height};
}

} // namespace screen::gfx::details