#if !defined(EVENT_HPP)
#define EVENT_HPP

#include "event_types.hpp"

namespace chippie
{

namespace event
{

/**
    @brief Register external handler
    For right now, only one handler per event type.
*/
void register_handler(event_handler) noexcept;
void register_event(event_type) noexcept;
void process_events(state&) noexcept;
[[nodiscard]] event_type dequeue_next_event() noexcept;

} // namespace event

} // namespace chippie

#endif