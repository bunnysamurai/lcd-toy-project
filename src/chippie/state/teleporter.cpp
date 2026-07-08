#include "teleporter.hpp"

namespace chippie
{

namespace
{
[[nodiscard]] constexpr uint32_t to_index(direction entry) noexcept
{
    switch (entry)
    {
    case direction::UP:
        return 0U;
    case direction::RIGHT:
        return 1U;
    case direction::DOWN:
        return 2U;
    case direction::LEFT:
        return 3U;
    }
    return 0U;
}
} // namespace

void teleporter::set_exit(direction entry_facing, Grid::Location exit_location, direction exit_facing) noexcept
{
    exit_locations[to_index(entry_facing)] = exit_location;
    exit_facings[to_index(entry_facing)] = exit_facing;
}

[[nodiscard]] std::pair<direction, Grid::Location> teleporter::compute_exit(direction entry_facing) const noexcept
{
    return std::make_pair(exit_facings[to_index(entry_facing)], exit_locations[to_index(entry_facing)]);
}

} // namespace chippie