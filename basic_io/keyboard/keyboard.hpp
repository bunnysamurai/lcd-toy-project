#if !defined(KEYBOARD_INPUT_H)
#define KEYBOARD_INPUT_H

#include <chrono>

namespace keyboard {
using duration = std::chrono::duration<uint32_t, std::ratio<1, 1'000'000>>;
enum struct result_t { SUCCESS, ERROR_TIMEOUT };

/** @brief wait for next key press 
 *
 * @param timeout Duration to wait.  Duration of 0 means wait forever.
 * @param[out] err Indicates if a key was retrieved, or a timeout occurred.
 *
 * @return the character
 */
[[nodiscard]] int wait_key(duration timeout, result_t &err) noexcept;
} // namespace keyboard

#endif