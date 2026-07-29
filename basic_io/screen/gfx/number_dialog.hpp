#ifndef SCREEN_GFX_NUMBER_DIALOG_HPP
#define SCREEN_GFX_NUMBER_DIALOG_HPP

#include <cstdint>
#include <span>

#include "defs.hpp"

namespace screen::gfx
{

struct number_dialog_palette
{
    uint16_t bright_highlight;
    uint16_t shadow_highlight;
    uint16_t background;
    uint16_t font_color;
};

class number_dialog final
{
  public:
    /** 
        @brief prompt string should be short
     */
    number_dialog(number_dialog_palette cfg, const char *promt, int minval, int maxval) noexcept;

    [[nodiscard]] int ask() noexcept;

  private:
    [[nodiscard]] int process_dialog() noexcept;
    [[nodiscard]] Rect compute_background_shape() noexcept;

    number_dialog_palette m_palette;
    const char *m_prompt;
    int m_min;
    int m_max;
};

} // namespace screen::gfx

#endif