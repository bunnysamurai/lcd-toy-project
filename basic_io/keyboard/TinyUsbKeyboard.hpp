#if !defined(TINYUSBKEYBOARD_HPP)
#define TINYUSBKEYBOARD_HPP

#include "keyboard_interface.h"

namespace keyboard::bsp
{
    using keyboard::duration;
    using keyboard::result_t;

    class TinyUsb_BasicKeyboard final
    {
    public:
        TinyUsb_BasicKeyboard() noexcept;
        [[nodiscard]] char wait_key(duration timeout, result_t &err) noexcept;

    private:
    };

}

namespace keyboard
{
    [[nodiscard]] char wait_key(duration, result_t &) noexcept;
}
#endif