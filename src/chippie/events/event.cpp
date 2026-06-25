#include "event.hpp"

#include <array>
#include <cstddef>

#include "event_types.hpp"

#include "embp/containers.hpp"

namespace chippie
{

namespace event
{

namespace
{

constexpr size_t MAX_EVENT_QUEUE_DEPTH{64};
embp::circular_array<event_type, MAX_EVENT_QUEUE_DEPTH> the_queue;
std::array<event_cb_t, static_cast<size_t>(event_type::MAX_EVENT_TYPE)> handlers;

[[nodiscard]] constexpr size_t asindex(event_type eventid) noexcept
{
    return static_cast<size_t>(eventid);
}

} // namespace

void register_handler(event_handler handle) noexcept
{
    handlers[asindex(handle.identifier)] = handle.handler;
}

void register_event(event_type eventid) noexcept
{
    the_queue.push_back(eventid);
}

void process_events() noexcept
{
    while (!the_queue.empty())
    {
        if (handlers[asindex(dequeue_next_event())])
        {
            handlers[asindex(dequeue_next_event())]();
        }
    }
}

[[nodiscard]] event_type dequeue_next_event() noexcept
{
    const auto eventid{the_queue.front()};
    the_queue.pop_front();
    return eventid;
}

} // namespace event

} // namespace chippie