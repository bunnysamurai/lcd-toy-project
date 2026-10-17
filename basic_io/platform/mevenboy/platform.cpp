#include "platform/platform.hpp"

#include "keyboard/TinyUsbKeyboard.hpp"

namespace platform
{
void init()
{
    keyboard::mevenboy::init();
}
} // namespace platform