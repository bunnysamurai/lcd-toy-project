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
    GOT_BURNED, /* stepped on campefire terrain */
    GAME_OVER,
    MOVE_MOVABLE_BLOCK,
    FLATTENED_BY_TANK,
    DANCED_BY_FIRE, /* touched a fire dancer */
    EXPLODED,       /* stepped on a bomb */
    GLIDED_OVER,    /* touched a water glider */
    ROLLED_BY_BALL, /* touched a purple ball */
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
