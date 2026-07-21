#if !defined(STATIC_MAP_HPP)
#define STATIC_MAP_HPP

#include "utils/Grid.hpp"
#include "terrain_types.hpp"

#include <array>
#include <cstdint>

namespace chippie
{

class static_map
{
  public:
    using map_element = terrain_type;
    static constexpr uint32_t map_width{32};
    static constexpr uint32_t map_height{32};
    std::array<map_element, map_width * map_height>
        map_data; /* to be loaded into RAM from ROM, as it'll just be easier if it can be mutable */

    [[nodiscard]] constexpr map_element &operator[](Grid::Location xy) noexcept
    {
        return map_data[to_linear_idx(xy)];
    }

    [[nodiscard]] constexpr const map_element &operator[](Grid::Location xy) const noexcept
    {
        return map_data[to_linear_idx(xy)];
    }

  private:
    [[nodiscard]] constexpr uint32_t to_linear_idx(Grid::Location xy) const noexcept
    {
        return xy.y * map_width + xy.x;
    }
};

} // namespace chippie

#endif