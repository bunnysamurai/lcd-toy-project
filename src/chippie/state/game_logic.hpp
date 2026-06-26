#if !defined(GAMELOGIC_HPP)
#define GAMELOGIC_HPP

#include "state.hpp"

#include "pico/time.h"

#include <cstdint>

namespace chippie
{

struct game_logic
{
  public:
    explicit game_logic(state &) noexcept;

    [[nodiscard]] bool is_active() noexcept;

    void process() noexcept;

  private:
    void move_entities() noexcept;

    [[nodiscard]] collision_result find_collisions(Grid::Location next_location) noexcept;

    void process_time_remaining() noexcept;

    state &game_state;
};

} // namespace chippie

#endif