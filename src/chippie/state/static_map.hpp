#if !defined(STATIC_MAP_HPP)
#define STATIC_MAP_HPP

#include "common/Grid.hpp"
#include "terrain_types.hpp"

#include <array>
#include <cstdint>

namespace chippie
{

class map_element
{
  private:
    terrain_type base : 7;
    bool moveable : 1;

  public:
    /* will clear moveable block */
    constexpr map_element &operator=(terrain_type type) noexcept
    {
        this->base = type;
        clear_moveable();
        return *this;
    }

    constexpr void set_moveable() noexcept
    {
        this->moveable = true;
    }

    constexpr void clear_moveable() noexcept
    {
        this->moveable = false;
    }

    constexpr operator terrain_type() const noexcept
    {
        if (this->moveable)
        {
            return terrain_type::MOVABLE_BLOCK;
        }
        return this->base;
    }
};

class static_map
{
  public:
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