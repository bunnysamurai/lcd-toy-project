#include "paint_utils.hpp"

#include <algorithm>
#include <array>

#include "chippie/entities/entity_types.hpp"
#include "chippie/textures/texture.hpp"
#include "embp/variable_array.hpp"
#include "screen/TileDef.h"
#include "screen/screen.hpp"

namespace chippie
{
namespace
{
/*
 _______   ______  _____ ____
|_   _\ \ / /  _ \| ____/ ___|
  | |  \ V /| |_) |  _| \___ \
  | |   | | |  __/| |___ ___) |
  |_|   |_| |_|   |_____|____/

*/

struct tile_map_t
{
    const screen::Tile *tile;
    entity_type entity;
};

/*
__     ___    ____  ____
\ \   / / \  |  _ \/ ___|
 \ \ / / _ \ | |_) \___ \
  \ V / ___ \|  _ < ___) |
   \_/_/   \_\_| \_\____/

*/

static constexpr uint32_t TILE_MAPPING_LIMIT{32};

embp::variable_array<tile_map_t, TILE_MAPPING_LIMIT> tile_map;

/*
 ____  ____  _____     ___  _____ _____
|  _ \|  _ \|_ _\ \   / / \|_   _| ____|
| |_) | |_) || | \ \ / / _ \ | | |  _|
|  __/|  _ < | |  \ V / ___ \| | | |___
|_|   |_| \_\___|  \_/_/   \_\_| |_____|

*/

} // namespace

/*
 ____  _   _ ____  _     ___ ____
|  _ \| | | | __ )| |   |_ _/ ___|
| |_) | | | |  _ \| |    | | |
|  __/| |_| | |_) | |___ | | |___
|_|    \___/|____/|_____|___\____|

*/

void draw_tile_index(uint32_t left_column, uint32_t top_row, texture::texture_type text) noexcept
{
    screen::draw_tile(left_column, top_row, texture::get_texture(text));
}

void draw_tile_index(const Grid &grid_def, Grid::Location location, texture::texture_type text) noexcept
{
    const auto [x, y]{grid_def.to_native(location)};

    draw_tile_index(x, y, text);
}

void draw_tile_index(uint32_t left_column, uint32_t top_row, entity_type entity) noexcept
{
    auto itr{std::find_if(std::begin(tile_map), std::end(tile_map),
                          [&](const auto &elem) { return elem.entity == entity; })};

    if (itr == std::end(tile_map))
    {
        return;
    }

    screen::draw_tile(left_column, top_row, *(itr->tile));
}

void draw_tile_index(const Grid &grid_def, Grid::Location location, entity_type entity) noexcept
{
    const auto [x, y]{grid_def.to_native(location)};

    draw_tile_index(x, y, entity);
}

mapping_result set_tile_to_entity_mapping(const screen::Tile *tile, entity_type entity) noexcept
{
    auto itr{std::find_if(std::begin(tile_map), std::end(tile_map),
                          [&](const auto &elem) { return elem.entity == entity; })};

    /* if an element is already in the map, reassign */
    if (itr != std::end(tile_map))
    {
        itr->tile = tile;
        return mapping_result::SUCCESS;
    }

    /* failed to find an element.  Check if map is full; if so, return failure. */
    if (tile_map.size() == tile_map.capacity())
    {
        return mapping_result::FAIL_MAP_FULL;
    }

    /* add a new entry to the list */
    tile_map.push_back(tile_map_t{.tile = tile, .entity = entity});

    return mapping_result::SUCCESS;
}

void clear_tile_to_entity_mapping() noexcept
{
    tile_map.clear();
}

} // namespace chippie