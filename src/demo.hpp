#if !defined(DEMO_HPP)
#define DEMO_HPP

namespace demo {

using callback_t = bool(*)(void);
void run_animation(callback_t stop_looping_callback);

} // namespace demo
#endif