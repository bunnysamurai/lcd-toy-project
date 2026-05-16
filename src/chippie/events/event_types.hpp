#if !defined(EVENT_TYPES_HPP)
#define EVENT_TYPES_HPP

namespace chippie
{
namespace event
{

enum struct event_type
{
    DECREMENT_CHIP_COUNT,
    GREEN_BUTTON,
    BLUE_BUTTON,
    RED_BUTTON,
    NEXT_LEVEL,
    RELEASE_ALL_TRAPS,
    GAME_COMPLETE,
    GAME_OVER
};

using event_cb_t = void (*)(void);

struct event_handler
{
    event_type identifier;
    event_cb_t handler;
};

} // namespace event

} // namespace chippie

#endif
