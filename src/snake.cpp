#include "snake.hpp"

#include <cstddef>
#include <cstdint>

#include "pico/time.h"

#include "screen/TileDef.h"
#include "screen/screen.hpp"

#include "embp/circular_array.hpp"

#include "snake_tiles_constexpr.hpp"

namespace {

using grid_t = uint8_t;
using pix_t = uint16_t;

/* Some game configurations */
// constexpr auto GAME_LOOP_MS{50};
constexpr auto GAME_TICK_US{100000U};
// constexpr auto APPLE_GROWTH_TICKS{3};

/* These abstractions should help with pixel->tile grid location
 * Observed behaviour of RattleRace(r) is that the snake moves on a fixed
 * grid of points that are larger than native screen resolution.
 *
 * We might also use this grid to draw where the walls, apples, ball, and
 * other snakes might be.
 */

struct ScreenLocation {
  pix_t x;
  pix_t y;
};

struct GridLocation {
  grid_t x;
  grid_t y;
};

struct TileTransform {
  pix_t off;
  pix_t scale;
};

struct TileGridCfg {
  TileTransform xdimension;
  TileTransform ydimension;

  grid_t grid_width;
  grid_t grid_height;
};

/* clang-format off */
/* the snake state:
 *    grid location of the head
 *    direction the head is pointed
 *    length of the body
 *    ring buffer of direction vectors, to build the snake body with
 *
 * snake behaviours:
 *    head always moves forward on every game tick, in the direction the head is pointed
 *    tail moves back along the body, 
 *      unless an apple was eaten, in which case it will remain stationary for N game ticks
 *
 */
/* clang-format on */
enum struct Direction : uint8_t { UP, RIGHT, DOWN, LEFT };

struct SnakeState {
  GridLocation head;
  Direction head_dir;
  embp::circular_array<Direction, 255U> body_vec;
};

TileGridCfg g_tile_grid;
SnakeState g_snake_state;

[[nodiscard]] ScreenLocation to_pixel_xy(GridLocation grid_xy) noexcept {
  /* TODO consider adding range checks here?  Or should it go somewhere else? */
  return {.x = static_cast<pix_t>(grid_xy.x * g_tile_grid.xdimension.scale +
                                  g_tile_grid.xdimension.off),
          .y = static_cast<pix_t>(grid_xy.y * g_tile_grid.ydimension.scale +
                                  g_tile_grid.ydimension.off)};
}

[[nodiscard]] constexpr GridLocation move_point(GridLocation point,
                                                Direction direction) noexcept {
  switch (direction) {
  case Direction::UP:
    point.y -= 1;
    break;
  case Direction::RIGHT:
    point.x += 1;
    break;
  case Direction::DOWN:
    point.y += 1;
    break;
  case Direction::LEFT:
    point.x -= 1;
    break;
  }
  return point;
}

[[nodiscard]] constexpr Direction get_opposite(Direction dir) noexcept {
  return static_cast<Direction>((static_cast<uint8_t>(dir) + 2U) & 0b11);
}

/** @brief initalize the snake state
 *
 *  NOTE: grid needs to be intialized before this function!
 */
void init_snake(GridLocation start, Direction dir) {
  g_snake_state.head = start;
  g_snake_state.head_dir = dir;
}

void clear_snake_tail() {
  /* iterate through the body_vec until the last one, which is the tail */
  auto head{g_snake_state.head};
  for (const auto dir : g_snake_state.body_vec) {
    head = move_point(head, dir);
  }
  const auto [pixx, pixy]{to_pixel_xy(head)};
  screen::draw_tile(pixx, pixy, snake::BackgroundTile);
}

void update_snake_state(bool growing) {
  /* head always moves */
  g_snake_state.head = move_point(g_snake_state.head, g_snake_state.head_dir);

  /* add to the length of the body, in the opposite direction */
  g_snake_state.body_vec.push_front(get_opposite(g_snake_state.head_dir));

  if (!growing) {
    /* also pop the tail location */
    clear_snake_tail();
    g_snake_state.body_vec.pop_back();
  }
}

void draw_snake() {

  /* for right now, we'll just draw green squares */

  /* draw the head */
  auto head{g_snake_state.head};
  const auto [pixx, pixy]{to_pixel_xy(head)};
  screen::draw_tile(pixx, pixy, snake::SnakeTile);

  /* iterate through the body_vec to draw the rest */
  for (const auto dir : g_snake_state.body_vec) {
    head = move_point(head, dir);
    const auto [pixx, pixy]{to_pixel_xy(head)};
    screen::draw_tile(pixx, pixy, snake::SnakeTile);
  }
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
    grid_t gy = 0;
    for (grid_t gx = 0; gx < g_tile_grid.grid_width; ++gx) {
      const auto [xx, yy]{to_pixel_xy({.x = gx, .y = gy})};
      screen::draw_tile(xx, yy, snake::BorderTile);
    }
    gy = g_tile_grid.grid_height - 1;
    for (grid_t gx = 0; gx < g_tile_grid.grid_width; ++gx) {
      const auto [xx, yy]{to_pixel_xy({.x = gx, .y = gy})};
      screen::draw_tile(xx, yy, snake::BorderTile);
    }
  }

  /* left and right borders */
  {
    grid_t gx = 0;
    for (grid_t gy = 1; gy < g_tile_grid.grid_height - 1; ++gy) {
      const auto [xx, yy]{to_pixel_xy({.x = gx, .y = gy})};
      screen::draw_tile(xx, yy, snake::BorderTile);
    }
    gx = g_tile_grid.grid_width - 1;
    for (grid_t gy = 1; gy < g_tile_grid.grid_height - 1; ++gy) {
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

void run() {
  init_the_screen();
  init_snake({.x = static_cast<grid_t>(g_tile_grid.grid_width >> 1),
              .y = static_cast<grid_t>(g_tile_grid.grid_height)},
             Direction::UP);

  uint8_t growing{2};
  absolute_time_t last_time{get_absolute_time()};
  /* game loop! */
  while (true) {
    const auto now{get_absolute_time()};
    if (absolute_time_diff_us(last_time, now) > GAME_TICK_US) {
      last_time = now;
      update_snake_state(growing > 0);

      if (growing > 0) {
        --growing;
      }

      draw_snake();
    }
  }
};

} // namespace snake