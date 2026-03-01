#include "screen_impl.hpp"

#include <iostream>
#include <span>

namespace screen_impl
{

/* =====================================================*/
/*           ____  _        _   _                       */
/*          / ___|| |_ __ _| |_(_) ___ ___              */
/*          \___ \| __/ _` | __| |/ __/ __|             */
/*           ___) | || (_| | |_| | (__\__ \             */
/*          |____/ \__\__,_|\__|_|\___|___/             */
/*                                                      */
/* =====================================================*/
namespace
{

std::span<const uint8_t> g_source_buffer;
Position g_virtual_topleft;
Dimensions g_virtual_size;
Format g_format;

std::span<const Clut> g_clut;

} // namespace

/* =====================================================*/
/*           ____        _     _ _                      */
/*          |  _ \ _   _| |__ | (_) ___                 */
/*          | |_) | | | | '_ \| | |/ __|                */
/*          |  __/| |_| | |_) | | | (__                 */
/*          |_|    \__,_|_.__/|_|_|\___|                */
/*                                                      */
/* =====================================================*/
[[nodiscard]] bool init(const uint8_t *video_buf, Position virtual_topleft, Dimensions virtual_size,
                        Format format) noexcept
{
    auto &&compute_min_span{[](const Position topleft, const Dimensions size) {
        return (size.width + topleft.column) * (size.height + topleft.row);
    }};

    g_source_buffer = std::span{video_buf, compute_min_span(virtual_topleft, virtual_size) * bitsizeof(format) / 8};
    g_virtual_topleft = virtual_topleft;
    g_virtual_size = virtual_size;
    g_format = format;

    return true;
}

void init_clut(const Clut *color_lut, uint32_t length) noexcept
{
    g_clut = std::span{color_lut, length};
}

[[nodiscard]] const uint8_t *get_video_buffer() noexcept
{
    return std::data(g_source_buffer);
}

void set_video_buffer(const uint8_t *buffer) noexcept
{
    if (!init(buffer, g_virtual_topleft, g_virtual_size, g_format))
    {
        std::cerr << "set_video_buffer.init() failed\n";
    }
}

[[nodiscard]] Format get_format() noexcept
{
    return g_format;
}

void set_format(Format fmt) noexcept
{
    if (!init(std::data(g_source_buffer), g_virtual_topleft, g_virtual_size, fmt))
    {
        std::cerr << "set_format.init() failed\n";
    }
}

[[nodiscard]] Dimensions get_virtual_screen_size() noexcept
{
    return g_virtual_size;
}

void set_virtual_screen_size(Position new_topleft, Dimensions new_size) noexcept
{
    if (!init(std::data(g_source_buffer), new_topleft, new_size, g_format))
    {
        std::cerr << "set_format.init() failed\n";
    }
}

[[nodiscard]] bool get_touch_report(TouchReport &) noexcept
{
    return false;
}

} // namespace screen_impl