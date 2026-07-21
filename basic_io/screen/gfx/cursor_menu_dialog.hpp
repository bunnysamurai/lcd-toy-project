#ifndef SCREEN_GFX_CURSOR_MENU_DIALOG_HPP
#define SCREEN_GFX_CURSOR_MENU_DIALOG_HPP

#include <cstdint>
#include <span>

#include "../../utils/Cursor.hpp"
#include "defs.hpp"

namespace screen::gfx
{

struct cursor_menu_dialog_palette
{
    uint16_t bright_highlight;
    uint16_t shadow_highlight;
    uint16_t background;
    uint16_t font_color;
};

/**
    Need to specify how many options and strings to describe those options,
    When "ask" is called, does the following:
        draws the menu on the screen
        processes user input
        when user "selects", returns the index in the menu that was selected

  TODO only supports screens in 4bpp mode
*/
class cursor_menu_dialog
{
  public:
    cursor_menu_dialog(cursor_menu_dialog_palette palette, const char *title,
                       std::span<const char *const> items) noexcept;

    int ask() noexcept;

  private:
    [[nodiscard]] Rect compute_background_shape() const noexcept;
    void draw_menu() const noexcept;
    [[nodiscard]] int process_menu() noexcept;
    void update_selection() noexcept;
    void draw_letter(const uint32_t xpos, const uint32_t ypos, const char letter) const noexcept;

    cursor_menu_dialog_palette m_palette;
    std::span<const char *const> m_items;
    const char *m_title;
    Rect m_menu_rect;
    Cursor<uint32_t> m_cursor;
    uint32_t m_prev_cursor;
};

} // namespace screen::gfx
#endif