#include "red_brown_buttons.hpp"
#include "chippie/entities/entity_types.hpp"

#include "chippie/entities/fire_dancer.hpp"
#include "chippie/entities/moveable_block.hpp"

#include <algorithm>

#include "pico/time.h"

#define DEBUG_PRINT
#ifdef DEBUG_PRINT
#include "pico/printf.h"
#endif

namespace chippie
{
namespace
{

[[nodiscard]] constexpr uint8_t find_next_uuid(const state &game_state) noexcept
{
    const auto maxval{std::ranges::max(game_state.entity_list, [](const auto &first, const auto &second) {
                          return first.uuid < second.uuid;
                      }).uuid};

    return maxval + 1;
}

} // namespace

void red_button::generate(state &game_state) noexcept
{
#ifdef DEBUG_PRINT
    printf("generating ");
#endif

    const auto next_uuid{find_next_uuid(game_state)};
#ifdef DEBUG_PRINT
    printf("with uuid %d ", next_uuid);
#endif

    switch (clone_type)
    {
    case entity_type::MOVEABLE_BLOCK:
#ifdef DEBUG_PRINT
        printf("a moveable block");
#endif
        game_state.entity_list.push_back(
            moveable_block_entity::create(game_state, clone_spawn, clone_facing, next_uuid));
        break;
    case entity_type::FIRE_DANCER:
#ifdef DEBUG_PRINT
        printf("a fire dancer");
#endif
        game_state.entity_list.push_back(fire_dancer::create(game_state, clone_spawn, clone_facing, next_uuid));
        break;
    default:
        break;
    }

    /* if this introduces a bit of drift, we probably won't notice... */
    game_state.entity_list.back().next_time =
        delayed_by_us(get_absolute_time(), get_entity_velocity(game_state.entity_list.back().identity));

#ifdef DEBUG_PRINT
    printf("\n");
#endif
}

bool brown_button::check_trap_is_set(const state &game_state) const noexcept
{
    auto itr{std::ranges::find_if(game_state.entity_list, [this](const auto &ent) { return ent.loc == button; })};

    return itr == std::cend(game_state.entity_list);
}

} // namespace chippie