#if !defined(STATIC_MAP_HPP)
#define STATIC_MAP_HPP

#include "common/Grid.hpp"
#include "terrain_types.hpp"

#include <array>
#include <cstdint>

/*
    "Frame" is information about the playfield that is contained in a matrix-like data structure.

    The entire play area of chippie can be described as a grid of squares 32x32 in size.

    The "static map", then, is a matrix of 32 columns by 32 rows, where each element
    in the matrix describes what type of square is on the play field.

    A square can be the following:
        CLEAR
        GRAVEL
        DIRT
        WALL | BLUE WALL | INVISIBLE WALL | TRIGGERED WALL
                                            (when Chip tries to step on a
                                             clear, it turns into a solid
                                             wall, preventing movement)
        BOMB
        CAMPFIRE
        WATER
        ICE
        GREEN_ARROW (4 orientations)
        LARGE GREY CIRCLE (when Chip steps on it, it turns into a solid wall, doesm not prevent movement)
        CHIP (could be many of them, up to 500 at least)


    Any square can be overlayed by an entity.  That logic is handled in the entity render loop.
    Overlays might be special enough that they get their own linked list to live in:
        GREEN DOOR
        YELLOW DOOR
        RED DOOR
        BLUE DOOR
        ICE CORNER (x4)
        THIN WALL (x4)


    Some of these levels have many, MANY of these... but we only have so much
    memory... but each level is only flush with a small set of square-types.

    So I think I will do something similar to the color LUT: there will be a square-type palette, then
    the "frame" only has to hold the index into the palette.  The palette will contain elements with enough bits
    to describe all of these square-types.  I can probably get away with a palette array of 16 elements.

*/
namespace chippie
{

class static_map
{
  private:
    struct complex_terrain_type
    {
        terrain_type base : 7;
        bool movable_block : 1;
    };

  public:
    static constexpr uint32_t map_width{32};
    static constexpr uint32_t map_height{32};
    std::array<complex_terrain_type, map_width * map_height>
        map_data; /* to be loaded into RAM from ROM, as it'll just be easier if it can be mutable */

    [[nodiscard]] constexpr terrain_type &operator[](Grid::Location xy) noexcept
    {
        return return_moveable_or_underlying(map_data[to_linear_idx(xy)]);
    }

    [[nodiscard]] constexpr const terrain_type &operator[](Grid::Location xy) const noexcept
    {
        return return_moveable_or_underlying(map_data[to_linear_idx(xy)]);
    }

    constexpr void set_moveable(Grid::Location xy) noexcept
    {
        map_data[to_linear_idx(xy)].movable_block = true;
    }

    constexpr void clear_moveable(Grid::Location xy) noexcept
    {
        map_data[to_linear_idx(xy)].movable_block = false;
    }

  private:
    [[nodiscard]] constexpr uint32_t to_linear_idx(Grid::Location xy) const noexcept
    {
        return xy.y * map_width + xy.x;
    }

    [[nodiscard]] static constexpr terrain_type return_moveable_or_underlying(complex_terrain_type data) noexcept
    {
        if (data.movable_block)
        {
            return terrain_type::MOVABLE_BLOCK;
        }
        return data.base;
    }
};

} // namespace chippie

#endif