#if !defined(EVENT_HPP)
#define EVENT_HPP

#include "event_types.hpp"

namespace chippie
{

namespace event
{

void register_handler(event_handler) noexcept;
void register_event(event_type) noexcept;
void process_events() noexcept;

[[nodiscard]] bool exit() noexcept;

} // namespace event

} // namespace chippie

#endif