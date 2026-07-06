#ifndef RED_BROWN_BUTTONS_HPP
#define RED_BROWN_BUTTONS_HPP

#include "chippie/chippie_common.hpp"
#include "chippie/entities/basic_entity.hpp"
#include "common/Grid.hpp"

namespace chippie
{
struct state;

struct red_button
{
  public:
    Grid::Location button;
    Grid::Location clone_spawn;
    direction clone_facing;
    entity_type clone_type;

    void generate(state &) noexcept;
};

struct brown_button
{
  public:
    Grid::Location button;
    Grid::Location trap;

    [[nodiscard]] bool check_trap_is_set(const state &) const noexcept;
};

} // namespace chippie
#endif