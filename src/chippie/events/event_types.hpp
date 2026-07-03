#if !defined(EVENT_TYPES_HPP)
#define EVENT_TYPES_HPP

#include "chippie/state/state.hpp"

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
    OPEN_MENU,
    DISPLAY_HINT,
    CLEAR_HINT,
    GAME_COMPLETE,
    TIME_UP,
    FELL_IN_WATER,
    EATEN_BY_BUG,
    GOT_BURNED,
    GAME_OVER,
    MOVE_MOVABLE_BLOCK,
    FLATTENED_BY_TANK,
    MAX_EVENT_TYPE
};

using event_cb_t = void (*)(state &);

struct event_handler
{
    event_type identifier;
    event_cb_t handler;
};

} // namespace event

} // namespace chippie

#endif
