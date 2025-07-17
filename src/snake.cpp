#include "snake.hpp"

#include <cstddef>
#include <cstdint>

#include "screen/TileDef.h"
#include "screen/screen.hpp"

#include "snake_tiles_constexpr.hpp"

namespace snake {
struct BorderTile;
}

namespace {

/* These abstractions should help with pixel->tile grid location
 * Observed behaviour of RattleRace(r) is that the snake moves on a fixed
 * grid of points that are larger than native screen resolution.
 *
 * We might also use this grid to draw where the walls, apples, ball, and
 * other snakes might be.
 */

struct ScreenLocation {
  uint16_t x;
  uint16_t y;
};

struct GridLocation {
  uint16_t x;
  uint16_t y;
};

struct TileTransform {
  uint16_t off;
  uint16_t scale;
};

struct TileGridCfg {
  TileTransform xdimension;
  TileTransform ydimension;

  uint16_t grid_width;
  uint16_t grid_height;
};

TileGridCfg g_tile_grid;

[[nodiscard]] ScreenLocation to_pixel_xy(GridLocation grid_xy) noexcept {
  /* TODO consider adding range checks here?  Or should it go somewhere else? */
  return {.x = static_cast<uint16_t>(grid_xy.x * g_tile_grid.xdimension.scale +
                                     g_tile_grid.xdimension.off),
          .y = static_cast<uint16_t>(grid_xy.y * g_tile_grid.ydimension.scale +
                                     g_tile_grid.ydimension.off)};
}
void configure_tile_grid() {
  const screen::Dimensions display_dims{screen::get_virtual_screen_size()};

  /* ... */
  /* for now, I'll assume the shape of the border tile drives the grid
   * requirements */

  const auto grid_scale{snake::BorderTile.side_length};
  const auto grid_width{display_dims.width / grid_scale};
  const auto grid_height{display_dims.height / grid_scale};
  const auto grid_xoff{(display_dims.width % grid_scale) >> 1};
  const auto grid_yoff{(display_dims.height % grid_scale) >> 1};

  g_tile_grid.grid_width = grid_width;
  g_tile_grid.grid_height = grid_height;
  g_tile_grid.xdimension.scale = grid_scale;
  g_tile_grid.xdimension.off = grid_xoff;
  g_tile_grid.ydimension.scale = grid_scale;
  g_tile_grid.ydimension.off = grid_yoff;
}

void draw_border() {
  /* border tiles all round the perimeter */

  /* top and bottom borders */
  {
    uint16_t gy = 0;
    for (uint16_t gx = 0; gx < g_tile_grid.grid_width; ++gx) {
      const auto [xx, yy]{to_pixel_xy({.x = gx, .y = gy})};
      screen::draw_tile(xx, yy, snake::BorderTile);
    }
    gy = g_tile_grid.grid_height - 1;
    for (uint16_t gx = 0; gx < g_tile_grid.grid_width; ++gx) {
      const auto [xx, yy]{to_pixel_xy({.x = gx, .y = gy})};
      screen::draw_tile(xx, yy, snake::BorderTile);
    }
  }

  /* left and right borders */
  {
    uint16_t gx = 0;
    for (uint16_t gy = 1; gy < g_tile_grid.grid_height - 1; ++gy) {
      const auto [xx, yy]{to_pixel_xy({.x = gx, .y = gy})};
      screen::draw_tile(xx, yy, snake::BorderTile);
    }
    gx = g_tile_grid.grid_width - 1;
    for (uint16_t gy = 1; gy < g_tile_grid.grid_height - 1; ++gy) {
      const auto [xx, yy]{to_pixel_xy({.x = gx, .y = gy})};
      screen::draw_tile(xx, yy, snake::BorderTile);
    }
  }

}

void init_the_screen() {
  /* introspect, then configure the tile grid */
  configure_tile_grid();

  /* we are a color application */
  screen::set_format(screen::Format::RGB565);

  /* we use a black background */
  screen::clear_screen();

  /* we setup a blue border */
  draw_border();
}
} // namespace

namespace snake {

void run() { init_the_screen(); }

} // namespace snake