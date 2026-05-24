#if !defined(PAINT_UTILS_HPP)
#define PAINT_UTILS_HPP

#include <cstdint>

#include "common/Grid.hpp"
#include "chippie/entities/entity_types.hpp"
#include "screen/TileDef.h"
#include "chippie/textures/texture.hpp"

namespace chippie
{

enum struct mapping_result
{
    SUCCESS,
    FAIL_MAP_FULL
};

/**
    @brief Draw the entity on the screen directly.
        Does require some setup, see set_tile_to_entity_mapping below.

        internally there is an array that maps entity_type to a tile data structure
        (see TETRIMINO_TILES in tetris_tiles_constexpr.hpp)
        This mapping array should be runtime configurable.
 */
void draw_tile_index(uint32_t left_column, uint32_t top_row, texture::texture_type text) noexcept;

/**
    @brief Draw entity on the screen, using the grid overlay.
 */
void draw_tile_index(const Grid &grid_def, Grid::Location location, texture::texture_type text) noexcept;

/**
    @brief Draw the entity on the screen directly.
        Does require some setup, see set_tile_to_entity_mapping below.

        internally there is an array that maps entity_type to a tile data structure
        (see TETRIMINO_TILES in tetris_tiles_constexpr.hpp)
        This mapping array should be runtime configurable.
 */
void draw_tile_index(uint32_t left_column, uint32_t top_row, entity_type entity) noexcept;

/**
    @brief Draw entity on the screen, using the grid overlay.
 */
void draw_tile_index(const Grid &grid_def, Grid::Location location, entity_type entity) noexcept;

/**
    @brief Set a tile<->entity relationship
        supports up to 2^N? tile<->entity mappings

    @return Whether or not the mapping was accepted.
 */
[[nodiscard]] mapping_result set_tile_to_entity_mapping(const screen::Tile *tile, entity_type entity) noexcept;

/**
    @brief Clear all entries in the mapping

    draw_tile_index will do nothing after this is called.
 */
void clear_tile_to_entity_mapping() noexcept;

} // namespace chippie

#endif
