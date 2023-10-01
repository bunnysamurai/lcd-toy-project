#if !defined(KEYBOARD_INPUT_H)
#define KEYBOARD_INPUT_H

#include <chrono>

namespace keyboard
{
    using duration = std::chrono::duration<uint32_t, std::ratio<1, 1'000'000>>;
    enum struct result_t
    {
        SUCCESS,
        ERROR_TIMEOUT
    };
}

#endif