#include "event.hpp"

#include <array>
#include <cstddef>

#include "event_types.hpp"

#include "embp/containers.hpp"

#define DEBUG_PRINT

namespace chippie
{

namespace event
{

namespace
{

constexpr size_t MAX_EVENT_QUEUE_DEPTH{64};
embp::circular_array<event_type, MAX_EVENT_QUEUE_DEPTH> the_queue;
std::array<event_cb_t, static_cast<size_t>(event_type::MAX_EVENT_TYPE)> handlers;

[[nodiscard]] const char *to_string(event_type evt) noexcept
{
    switch (evt)
    {
    case event_type::DECREMENT_CHIP_COUNT:
        return "DECREMENT_CHIP_COUNT";
    case event_type::GREEN_BUTTON:
        return "GREEN_BUTTON";
    case event_type::BLUE_BUTTON:
        return "BLUE_BUTTON";
    case event_type::RED_BUTTON:
        return "RED_BUTTON";
    case event_type::NEXT_LEVEL:
        return "NEXT_LEVEL";
    case event_type::RELEASE_ALL_TRAPS:
        return "RELEASE_ALL_TRAPS";
    case event_type::OPEN_MENU:
        return "OPEN_MENU";
    case event_type::DISPLAY_HINT:
        return "DISPLAY_HINT";
    case event_type::CLEAR_HINT:
        return "CLEAR_HINT";
    case event_type::GAME_COMPLETE:
        return "GAME_COMPLETE";
    case event_type::TIME_UP:
        return "TIME_UP";
    case event_type::FELL_IN_WATER:
        return "FELL_IN_WATER";
    case event_type::GAME_OVER:
        return "GAME_OVER";
    case event_type::MOVE_MOVABLE_BLOCK:
        return "MOVE_MOVABLE_BLOCK";
    case event_type::FLATTENED_BY_TANK:
        return "FLATTENED_BY_TANK";
    case event_type::DANCED_BY_FIRE:
        return "DANCED_BY_FIRE";
    case event_type::EXPLODED:
        return "EXPLODED";
    case event_type::GLIDED_OVER:
        return "GLIDED_OVER";
    case event_type::ROLLED_BY_BALL:
        return "ROLLED_BY_BALL";
    case event_type::CHOMPED_BY_FROG:
        return "CHOMPED_BY_FROG";
    case event_type::SMUSHED:
        return "SMUSHED";
    default:
        return "OTHER";
    }

    return "OTHER";
}

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
#ifdef DEBUG_PRINT
    printf("registered event %s\n", to_string(eventid));
#endif
    the_queue.push_back(eventid);
}

void process_events(state &game_state) noexcept
{
    while (!the_queue.empty())
    {
        if (handlers[asindex(dequeue_next_event())])
        {
            handlers[asindex(dequeue_next_event())](game_state);
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