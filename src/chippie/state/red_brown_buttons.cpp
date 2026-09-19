#include "red_brown_buttons.hpp"

#include "chippie/chippie_common.hpp"
#include "chippie/entities/basic_entity.hpp"
#include "chippie/entities/entity_types.hpp"
#include "chippie/state/state.hpp"

#include <algorithm>

// #define DEBUG_PRINT
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
    const auto next_uuid{find_next_uuid(game_state)};

    /* should guarentee the new entity will move within the game_logic processing loop it was added */
    auto &chippie{game_state.entity_list.front()};
    const uint64_t next_move_time{chippie.next_time - get_entity_velocity(chippie.identity)};

    game_state.entity_list.push_back(
        create_entity(clone_type, clone_spawn, clone_facing, next_uuid, game_state, next_move_time));

    /* this supresses an immediate move, which otherwise looks weird on the display */
    game_state.entity_list.back().has_summoning_sickness = true;

}

bool brown_button::check_trap_is_set(const state &game_state) const noexcept
{
    auto itr{std::ranges::find_if(game_state.entity_list, [this](const auto &ent) { return ent.loc == button; })};

    return itr == std::cend(game_state.entity_list);
}

} // namespace chippie