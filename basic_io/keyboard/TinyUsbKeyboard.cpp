#include "TinyUsbKeyboard.hpp"

namespace
{
    keyboard::bsp::TinyUsb_BasicKeyboard g_tinyusb_key;
}

namespace keyboard::bsp
{
    TinyUsb_BasicKeyboard::TinyUsb_BasicKeyboard() noexcept {}
    [[nodiscard]] char TinyUsb_BasicKeyboard::wait_key(duration timeout, result_t &err) noexcept { return '\0'; }
}

namespace keyboard
{
    [[nodiscard]] char wait_key(duration timeout, result_t &error) noexcept
    {
        return g_tinyusb_key.wait_key(timeout, error);
    }
}