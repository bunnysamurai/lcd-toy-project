#if !defined(VIEW_GRID_HPP)
#define VIEW_GRID_HPP

#include "common/Grid.hpp"

namespace chippie
{

[[nodiscard]] bool within_view_grid(Grid::Location) noexcept;

}

#endif