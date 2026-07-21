#ifndef TELEPORTER_HPP
#define TELEPORTER_HPP

#include "chippie/chippie_common.hpp"
#include "utils/Grid.hpp"

#include <array>
#include <utility>

namespace chippie
{

struct teleporter
{
    Grid::Location entry_location;
    std::array<Grid::Location, 4> exit_locations;
    std::array<direction, 4> exit_facings;

    void set_exit(direction entry_facing, Grid::Location exit_location, direction exit_facing) noexcept;

    [[nodiscard]] std::pair<direction, Grid::Location> compute_exit(direction entry_facing) const noexcept;
};

} // namespace chippie

#endif
