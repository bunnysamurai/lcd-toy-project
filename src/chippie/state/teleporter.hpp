#ifndef TELEPORTER_HPP
#define TELEPORTER_HPP

#include "chippie/chippie_common.hpp"
#include "chippie/entities/basic_entity.hpp"
#include "embp/circular_array.hpp"
#include "embp/variable_array.hpp"
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

/*
Based on my conversation with Sam, teleporters work quite differently from what I first suspected...
The good news: the setup for the teleporters is a lot simpler: I just need to know what order they go in
The bad news: the logic to handle where to exit is more complicated... there are a few rules governing
where the exit will be:
  1. Teleporters are a chain (linked list) of locations on the map
  1. You enter in one direction...
  1. And exit on the opposite side of the next teleporter in the linked list
  1. If you cannot exit (because you collide with an opaque tile),
     you exit on the opposite side of the next teleporter after that in the linked list
  1. Repeat until you either
    1. find a valid exit
    1. return to where you started, in which case, your exit is where you entered with an opposite facing
 */

struct state; /* forward declaration... sigh... */

struct teleporter_chain
{
    embp::variable_array<Grid::Location, 9> locations;

    [[nodiscard]] std::pair<direction, Grid::Location> compute_exit(const entity &this_ent,
                                                                    Grid::Location entry_location,
                                                                    direction entry_facing) const noexcept;
};

} // namespace chippie

#endif
