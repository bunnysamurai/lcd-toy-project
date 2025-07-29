#include "snake.hpp"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iterator>

#include "pico/rand.h"
#include "pico/time.h"

#ifdef BSIO_KEYBOARD_AVAILABLE
#include "keyboard/keyboard.hpp"
#else
#include "pico/stdio.h"
#endif

#include "screen/TileDef.h"
#include "screen/screen.hpp"

#include "embp/circular_array.hpp"

#include "snake_common.hpp"
#include "snake_levels_constexpr.hpp"
#include "snake_tiles_constexpr.hpp"

namespace {

using snake::Direction;
using snake::grid_t;
using snake::pix_t;

/* Some game configurations */
constexpr std::chrono::milliseconds KEYBOARD_POLL_MS{
    1}; /* basically how often we poll for input */
constexpr auto GAME_TICK_US{200000U};
constexpr auto APPLE_GROWTH_TICKS{3U};
constexpr auto NUMBER_OF_APPLES{10U};

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

[[nodiscard]] constexpr bool operator==(GridLocation lhs,
                                        GridLocation rhs) noexcept {
  return lhs.x == rhs.x && lhs.y == rhs.y;
}

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

struct SnakeState {
  GridLocation head;
  Direction head_dir;
  embp::circular_array<Direction, 255U> body_vec;
};

struct LevelState {
  bool exit_is_open;
  GridLocation exit;
  snake::Level lvl;
};

TileGridCfg g_tile_grid;
SnakeState g_snake_state;
LevelState g_level;

[[nodiscard]] ScreenLocation to_pixel_xy(GridLocation grid_xy) noexcept {
  /* TODO consider adding range checks here?  Or should it go somewhere else? */
  return {.x = static_cast<pix_t>(grid_xy.x * g_tile_grid.xdimension.scale +
                                  g_tile_grid.xdimension.off),
          .y = static_cast<pix_t>(grid_xy.y * g_tile_grid.ydimension.scale +
                                  g_tile_grid.ydimension.off)};
}

/** @brief Advances a GridLocation in a given Direction
 *
 * @param point Grid Point
 * @param direction Up, Down, Left, or Right
 *
 * @return A new GridLocation, advanced in the specified direction.
 */
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

/** @brief Converts a Direction to it's 4-way opposite.
 *
 * @param dir A direction, Up, Down, Left or Right
 *
 * @return A direction opposite of dir, Down, Up, Right or Left
 */
[[nodiscard]] constexpr Direction get_opposite(Direction dir) noexcept {
  return static_cast<Direction>((static_cast<uint8_t>(dir) + 2U) & 0b11);
}

[[nodiscard]] constexpr bool is_updown(Direction dir) noexcept {
  return !(static_cast<uint8_t>(dir) & 0x1);
}

/* ========================================================== */
/*                _                   _                       */
/*               | |    _____   _____| |                      */
/*               | |   / _ \ \ / / _ \ |                      */
/*               | |__|  __/\ V /  __/ |                      */
/*               |_____\___| \_/ \___|_|                      */
/*                                                            */
/* ========================================================== */
void init_level(snake::Level lvl) noexcept {
  g_level.exit_is_open = false;
  g_level.exit = {.x = static_cast<grid_t>(g_tile_grid.grid_width >> 1),
                  .y = 0};
  g_level.lvl = lvl;
}

void draw_straight_line(snake::StraightLine line, const screen::Tile &tile) {
  GridLocation start{.x = line.xs, .y = line.ys};
  snake::Direction dir{line.dir};

  int8_t xinc{};
  int8_t yinc{};
  switch (dir) {
  case snake::Direction::DOWN:
    yinc = 1;
    break;
  case snake::Direction::UP:
    yinc = -1;
    break;
  case snake::Direction::RIGHT:
    xinc = 1;
    break;
  case snake::Direction::LEFT:
    xinc = -1;
    break;
  }

  int32_t count{line.len};
  while (count > 0) {
    const auto [pixx, pixy]{to_pixel_xy(start)};
    screen::draw_tile(pixx, pixy, tile);
    --count;
    start.x += xinc;
    start.y += yinc;
  }
}
void draw_straight_lines(const uint8_t *p_data, uint32_t len) {
  auto p_line_data{p_data};
  for (uint32_t ii = 0; ii < len; ++ii) {
    draw_straight_line(snake::decode_straight_line(p_line_data),
                       snake::BorderTile);
    std::advance(p_line_data, 3); /* TODO 3 is a magic number... */
  }
}
void draw_this_level(snake::Level lvl) noexcept {
  const auto *p_structure{lvl.data};

  for (uint32_t ii = 0; ii < lvl.len; ++ii) {
    switch (p_structure[ii].type) {
    case snake::StructureType::STRAIGHT_LINE:
      draw_straight_lines(p_structure[ii].data, p_structure[ii].len);
      break;
    default:
      /* TODO not yet implemented */
      break;
    }
  }
}
/* ========================================================== */
/*     ____  _   _    _        _        _    _  _______ _     */
/*    / ___|| \ | |  / \      / \      / \  | |/ / ____| |    */
/*    \___ \|  \| | / _ \    / _ \    / _ \ | ' /|  _| | |    */
/*     ___) | |\  |/ ___ \  / ___ \  / ___ \| . \| |___|_|    */
/*    |____/|_| \_/_/   \_\/_/   \_\/_/   \_\_|\_\_____(_)    */
/*                                                            */
/* ========================================================== */
/** @brief initalize the snake state
 *
 *  NOTE: grid needs to be intialized before this function!
 */
void init_snake(GridLocation start, Direction dir) {
  g_snake_state.head = start;
  g_snake_state.head_dir = dir;
  g_snake_state.body_vec.clear();
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

void impl_draw_head(const screen::Tile &tile) {
  /* draw the head */
  auto head{g_snake_state.head};
  const auto [pixx, pixy]{to_pixel_xy(head)};
  screen::draw_tile(pixx, pixy, tile);
}

[[nodiscard]] constexpr screen::Tile
determine_snake_head_tile(Direction head_dir) noexcept {
  switch (head_dir) {
  case Direction::UP:
    return to_snake_tile(snake::SnakeBodyPart::HEAD_UP);
  case Direction::DOWN:
    return to_snake_tile(snake::SnakeBodyPart::HEAD_DOWN);
  case Direction::LEFT:
    return to_snake_tile(snake::SnakeBodyPart::HEAD_LEFT);
  case Direction::RIGHT:
    return to_snake_tile(snake::SnakeBodyPart::HEAD_RIGHT);
  }
  return to_snake_tile(
      snake::SnakeBodyPart::HEAD_RIGHT); /* technically unreachable, if warnings
                                            are correctly enabled */
}

[[nodiscard]] constexpr screen::Tile
determine_snake_tail_tile(Direction dir) noexcept {
  switch (dir) {
  case Direction::UP:
    return to_snake_tile(snake::SnakeBodyPart::TAIL_UP);
  case Direction::DOWN:
    return to_snake_tile(snake::SnakeBodyPart::TAIL_DOWN);
  case Direction::LEFT:
    return to_snake_tile(snake::SnakeBodyPart::TAIL_LEFT);
  case Direction::RIGHT:
    return to_snake_tile(snake::SnakeBodyPart::TAIL_RIGHT);
  }
  return to_snake_tile(snake::SnakeBodyPart::TAIL_RIGHT); /* technically
                                              unreachable, if warnings are
                                              cranked up high enough */
}

[[nodiscard]] constexpr screen::Tile
determine_snake_body_tile(Direction previous, Direction next) noexcept {
  /* We do end up with a table:
   *
   *        | UP  | RIGHT | DOWN | LEFT |
   *        +-----|-------|------|------+
   *    UP  | X   | DWRHT | UPDN | DWLF |
   *  RIGHT | UPL |   X   | DWLF | LFRT |
   *   DOWN | UPD | UPRHT |  X   | UPLF |
   *   LEFT | UPR | LFRHT | DWRT |  X   |
   */

  /* clang-format off */
  constexpr std::array<snake::SnakeBodyPart, 16> LUT{
    snake::SnakeBodyPart::BODY_UPDOWN, snake::SnakeBodyPart::BODY_DOWNRIGHT, snake::SnakeBodyPart::BODY_UPDOWN, snake::SnakeBodyPart::BODY_DOWNLEFT,
    snake::SnakeBodyPart::BODY_UPLEFT, snake::SnakeBodyPart::BODY_LEFTRIGHT, snake::SnakeBodyPart::BODY_DOWNLEFT, snake::SnakeBodyPart::BODY_LEFTRIGHT,
    snake::SnakeBodyPart::BODY_UPDOWN, snake::SnakeBodyPart::BODY_UPRIGHT, snake::SnakeBodyPart::BODY_UPDOWN, snake::SnakeBodyPart::BODY_UPLEFT,
    snake::SnakeBodyPart::BODY_UPRIGHT, snake::SnakeBodyPart::BODY_LEFTRIGHT, snake::SnakeBodyPart::BODY_DOWNRIGHT, snake::SnakeBodyPart::BODY_LEFTRIGHT
  };
  /* clang-format on */

  const auto idx{static_cast<uint8_t>(previous) * 4 +
                 static_cast<uint8_t>(next)};

  return snake::to_snake_tile(LUT[idx]);
}

void impl_draw_along_the_body(const screen::Tile &tile) noexcept {
  auto head{g_snake_state.head};
  /* iterate through the body_vec to draw the rest */
  auto itr{g_snake_state.body_vec.begin()};
  for (const auto dir : g_snake_state.body_vec) {
    head = move_point(head, dir);
    const auto [pixx, pixy]{to_pixel_xy(head)};
    screen::draw_tile(pixx, pixy, tile);
  }
}

void impl_draw_the_snake_body() noexcept {
  /* if the snake is only length 1, this is a special case */
  if (g_snake_state.body_vec.size() == 0) {
    return;
  }

  /* iterate through the body_vec to draw the rest */
  auto head{g_snake_state.head};
  auto itr{g_snake_state.body_vec.begin()};
  while (itr != std::next(g_snake_state.body_vec.end(), -1)) {
    const auto dir{*itr};
    const auto nextdir{*(std::next(itr, 1))};
    const auto tile{determine_snake_body_tile(dir, nextdir)};
    head = move_point(head, dir);
    const auto [pixx, pixy]{to_pixel_xy(head)};
    screen::draw_tile(pixx, pixy, tile);
    itr++;
  }

  /* draw the tail */
  const auto dir{*itr};
  const auto tile{determine_snake_tail_tile(dir)};
  head = move_point(head, dir);
  const auto [pixx, pixy]{to_pixel_xy(head)};
  screen::draw_tile(pixx, pixy, tile);
}

void cleanup_the_body() { impl_draw_along_the_body(snake::BackgroundTile); }

void draw_snake() {
  /* for right now, we'll just draw green squares */
  impl_draw_head(determine_snake_head_tile(g_snake_state.head_dir));
  impl_draw_the_snake_body();
}

/**
 * @return True if snake changed direction, false otherwise
 */
bool change_snake_direction(int key_pressed) noexcept {
  /*
   *  validate the character pressed
   *  convert character to Direction
   *  verify Direction is a valid option
   *    if so, update the snake's direction
   *
   * Until I figure out UTF-8, we'll use:
   *  I - up
   *  J - left
   *  K - down
   *  L - right
   *
   * Rules for changind direction are simple:
   *   If moving up/down, then only left/right is valid
   *   If moving right/left, then only up/down is valid
   */

  const auto key{static_cast<char>(key_pressed)};

  if (is_updown(g_snake_state.head_dir)) {
    switch (key) {
    case 'j':
      g_snake_state.head_dir = Direction::LEFT;
      return true;
    case 'l':
      g_snake_state.head_dir = Direction::RIGHT;
      return true;
    default:
      /* do nothing */
      break;
    }
  } else {
    switch (key) {
    case 'i':
      g_snake_state.head_dir = Direction::UP;
      return true;
    case 'k':
      g_snake_state.head_dir = Direction::DOWN;
      return true;
    default:
      /* do nothing */
      break;
    }
  }
  return false;
}

/* ================================================================= */
/*    ____      _     _    ___     ____                _             */
/*   / ___|_ __(_) __| |  ( _ )   | __ )  ___  _ __ __| | ___ _ __   */
/*  | |  _| '__| |/ _` |  / _ \/\ |  _ \ / _ \| '__/ _` |/ _ \ '__|  */
/*  | |_| | |  | | (_| | | (_>  < | |_) | (_) | | | (_| |  __/ |     */
/*   \____|_|  |_|\__,_|  \___/\/ |____/ \___/|_|  \__,_|\___|_|     */
/*                                                                   */
/* ================================================================= */
void update_lives_on_screen(uint8_t lives) noexcept {
  constexpr auto HEADTILE{snake::to_snake_tile(snake::SnakeBodyPart::HEAD_UP)};
  constexpr auto BODYTILE{
      snake::to_snake_tile(snake::SnakeBodyPart::BODY_UPDOWN)};
  constexpr auto TAILTILE{
      snake::to_snake_tile(snake::SnakeBodyPart::TAIL_DOWN)};
  static_assert(HEADTILE.side_length == BODYTILE.side_length);
  static_assert(HEADTILE.side_length == TAILTILE.side_length);
  constexpr auto TILE_INC{HEADTILE.side_length};

  const screen::Dimensions display_dims{screen::get_virtual_screen_size()};
  const auto row_start{
      ((display_dims.height - display_dims.width) - TILE_INC) >> 1};
  const auto col_inc{(TILE_INC << 1) - 1};

  if (lives > 0) {
    --lives;
  }

  auto col_start{TILE_INC >> 1};
  for (int ii = 0; ii < 5; ++ii) {
    screen::draw_tile(col_start + ii * col_inc, row_start - TILE_INC,
                      snake::BackgroundTile);
    screen::draw_tile(col_start + ii * col_inc, row_start,
                      snake::BackgroundTile);
    screen::draw_tile(col_start + ii * col_inc, row_start + TILE_INC,
                      snake::BackgroundTile);
  }
  for (; lives > 0; --lives) {
    screen::draw_tile(col_start, row_start - TILE_INC, HEADTILE);
    screen::draw_tile(col_start, row_start, BODYTILE);
    screen::draw_tile(col_start, row_start + TILE_INC, TAILTILE);
    col_start += col_inc;
  }
}

void configure_tile_grid() noexcept {
  const screen::Dimensions display_dims{screen::get_virtual_screen_size()};

  /*
   * Play area must be square.  The grid must be 33x33.
   */
  const auto screen_pix_y_off{display_dims.height - display_dims.width};
  const auto screen_pix_height{display_dims.height - screen_pix_y_off};

  /* for now, I'll assume the shape of the border tile drives the grid
   * requirements */

  const auto grid_scale{snake::BorderTile.side_length};
  const auto grid_width{display_dims.width / grid_scale};
  const auto grid_height{screen_pix_height / grid_scale};
  const auto grid_xoff{(display_dims.width % grid_scale) >> 1};
  const auto grid_yoff{(screen_pix_height % grid_scale) >> 1};

  g_tile_grid.grid_width = grid_width;
  g_tile_grid.grid_height = grid_height;
  g_tile_grid.xdimension.scale = grid_scale;
  g_tile_grid.xdimension.off = grid_xoff;
  g_tile_grid.ydimension.scale = grid_scale;
  g_tile_grid.ydimension.off = screen_pix_y_off + grid_yoff;
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
void update_borders(GridLocation loc) noexcept {
  const auto [xx, yy]{to_pixel_xy(loc)};
  screen::draw_tile(xx, yy, snake::BorderTile);

  if (g_level.exit_is_open) {
    const auto [xx, yy]{to_pixel_xy(g_level.exit)};
    screen::draw_tile(xx, yy, snake::BackgroundTile);
  } else {
    const auto [xx, yy]{to_pixel_xy(g_level.exit)};
    screen::draw_tile(xx, yy, snake::BorderTile);
  }
}

void init_the_screen() noexcept {
  /* introspect, then configure the tile grid */
  configure_tile_grid();

  /* we are a color application */
  screen::set_format(snake::TILE_FORMAT);
  screen::init_clut(snake::Palette.data(), snake::Palette.size());

  /* we use a black background */
  screen::clear_screen();

  /* we setup a blue border */
  draw_border();
}

/* ===================================================== */
/*      _                _         ____           _      */
/*     / \   _ __  _ __ | | ___   / ___|__ _ _ __| |_    */
/*    / _ \ | '_ \| '_ \| |/ _ \ | |   / _` | '__| __|   */
/*   / ___ \| |_) | |_) | |  __/ | |__| (_| | |  | |_    */
/*  /_/   \_\ .__/| .__/|_|\___|  \____\__,_|_|   \__|   */
/*          |_|   |_|                                    */
/* ===================================================== */
embp::variable_array<GridLocation, NUMBER_OF_APPLES> g_apple_locations;

void init_apples() noexcept {
  /* if there are already apples, i.e. the level restarted, clear the old ones
   */
  for (const auto apple : g_apple_locations) {
    const auto [xx, yy]{to_pixel_xy(apple)};
    screen::draw_tile(xx, yy, snake::BackgroundTile);
  }

  /* just places an apple somewhere */
  g_apple_locations.resize(NUMBER_OF_APPLES);
  for (auto &apple : g_apple_locations) {
    const uint32_t seed{get_rand_32()};
    const auto xloc{(seed & 0xFFFFU) % (g_tile_grid.grid_width - 2) + 1};
    const auto yloc{((seed >> 16) & 0xFFFFU) % (g_tile_grid.grid_height - 2) +
                    1};
    apple = {.x = static_cast<grid_t>(xloc), .y = static_cast<grid_t>(yloc)};
  }

  /* and draw the apples */
  for (const auto apple : g_apple_locations) {
    const auto [xx, yy]{to_pixel_xy(apple)};
    screen::draw_tile(xx, yy, snake::AppleTile);
  }
}

[[nodiscard]] uint32_t get_number_of_apples() noexcept {
  return g_apple_locations.size();
}

void remove_apple(uint32_t apple_idx) noexcept {
  const auto itr{std::next(g_apple_locations.begin(), apple_idx)};
  g_apple_locations.erase(itr);
}

/**
 * What's the logic here?  Hmm...
 */
void add_apple() noexcept {
  static_assert(sizeof(grid_t) == 1,
                "add_apple only works with 8-bit grid points right meow");

  const uint32_t seed{get_rand_32()};

  /* Some rules:
   *   We never put an apple within N points of the head
   *   We put the apple in a quadrant the head is not
   *   can't put an apple on any border points
   */

  /* first, figure out which quadrant the snake is in
   * next, pick a quadrant by
   *   counting the number of set bits in the top 3 bits of the seed.
   *   add the number of set bits to the snake's current quadrant, mod 4
   * then, divide this quadrant by 8 on each side by:
   *    say the quadrant is X by Y points in size
   *    ([2:0] * X + 4) / 8 = grid point x
   *    ([5:3] * Y + 4) / 8 = grid point y
   */
}

/* ================================================================= */
/*   ____      _ _ _     _               _                _          */
/*  / ___|___ | | (_)___(_) ___  _ __   | |    ___   __ _(_) ___     */
/* | |   / _ \| | | / __| |/ _ \| '_ \  | |   / _ \ / _` | |/ __|    */
/* | |__| (_) | | | \__ \ | (_) | | | | | |__| (_) | (_| | | (__     */
/*  \____\___/|_|_|_|___/_|\___/|_| |_| |_____\___/ \__, |_|\___|    */
/*                                                  |___/            */
/* ================================================================= */
enum struct Collision { NONE, BORDER, APPLE, SNAKE, EXIT };
uint32_t g_collided_apple;

[[nodiscard]] bool
check_for_apple_collision(uint32_t &collided_apple_index) noexcept {
  uint32_t idx{0};
  for (const auto apple : g_apple_locations) {
    if (apple == g_snake_state.head) {
      collided_apple_index = idx;
      return true;
    }
    ++idx;
  }
  return false;
}

[[nodiscard]] bool check_for_itself_collision() noexcept {
  /* if the head is equal to any of it's body, we have a collision */
  const auto head{g_snake_state.head};
  auto start{head};
  for (const auto dir : g_snake_state.body_vec) {
    start = move_point(start, dir);
    if (start == head) {
      return true;
    }
  }
  return false;
}

[[nodiscard]] constexpr bool
check_for_straight_line_collisions(const uint8_t *p_data,
                                   uint32_t len) noexcept {
  const GridLocation point{g_snake_state.head};
  auto p_line_data{p_data};
  bool status{false};
  for (uint32_t ii = 0; ii < len; ++ii) {
    const snake::StraightLine linedef{snake::decode_straight_line(p_line_data)};
    const GridLocation line_start{.x = linedef.xs, .y = linedef.ys};
    switch (linedef.dir) {
    case snake::Direction::DOWN:
      status = point.x == linedef.xs && point.y >= linedef.ys &&
               point.y < linedef.ys + linedef.len;
      break;
    case snake::Direction::RIGHT:
      status = point.y == linedef.ys && point.x >= linedef.xs &&
               point.x < linedef.xs + linedef.len;
      break;
    case snake::Direction::UP:
      status = point.x == linedef.xs &&
               point.y >= linedef.ys - linedef.len + 1 && point.y <= linedef.ys;
      break;
    case snake::Direction::LEFT:
      status = point.y == linedef.ys &&
               point.x >= linedef.xs - linedef.len + 1 && point.x <= linedef.xs;
      break;
    }
    if (status) {
      return true;
    }
    std::advance(p_line_data, 3); /* TODO 3 is a magic number... */
  }
  return false;
}

[[nodiscard]] constexpr bool
check_for_level_collisions(snake::Level lvl) noexcept {
  const auto *p_structure{lvl.data};
  for (uint32_t ii = 0; ii < lvl.len; ++ii) {
    switch (p_structure[ii].type) {
    case snake::StructureType::STRAIGHT_LINE:
      if (check_for_straight_line_collisions(p_structure[ii].data,
                                             p_structure[ii].len)) {
        return true;
      }
    default:
      /* TODO implement the rest... */
      break;
    }
  }
  return false;
}

[[nodiscard]] Collision check_for_collisions() noexcept {
  /* If head is on the exit, and the door is open, then make like a tree and get
   * outta here. */
  if (g_level.exit_is_open && g_snake_state.head == g_level.exit) {
    return Collision::EXIT;
  }

  /* if head is on the border, whoops! Game over. */
  if (g_snake_state.head.x < 1 ||
      g_snake_state.head.x > g_tile_grid.grid_width - 2 ||
      g_snake_state.head.y < 1 ||
      g_snake_state.head.y > g_tile_grid.grid_height - 2 ||
      check_for_level_collisions(g_level.lvl)) {
    return Collision::BORDER;
  }

  /* if the head is on itself, whoa!  No cannabilism allowed. */
  if (check_for_itself_collision()) {
    return Collision::SNAKE;
  }
  /* if head is on an apple, yum! Remove the apple. */
  if (check_for_apple_collision(g_collided_apple)) {
    return Collision::APPLE;
  }

  return Collision::NONE;
}

/* ======================================================= */
/*     _   _                 ___                   _       */
/*    | | | |___  ___ _ __  |_ _|_ __  _ __  _   _| |_     */
/*    | | | / __|/ _ \ '__|  | || '_ \| '_ \| | | | __|    */
/*    | |_| \__ \  __/ |     | || | | | |_) | |_| | |_     */
/*     \___/|___/\___|_|    |___|_| |_| .__/ \__,_|\__|    */
/*                                    |_|                  */
/* ======================================================= */
enum struct UserInput { QUIT, PLAY, CHANGE_DIRECTION };

UserInput process_user_input() {
  UserInput user_has_input{UserInput::PLAY};
#ifdef BSIO_KEYBOARD_AVAILABLE
  keyboard::result_t key_status;
  const int key_pressed{keyboard::wait_key(KEYBOARD_POLL_MS, key_status)};
  if (key_status == keyboard::result_t::SUCCESS) {
    if (key_pressed == 'q') {
      return UserInput::QUIT;
    }
    if (change_snake_direction(key_pressed)) {
      user_has_input = UserInput::CHANGE_DIRECTION;
    }
  }
#else
  const auto key_pressed{stdio_getchar_timeout_us(
      std::chrono::duration_cast<std::chrono::microseconds>(KEYBOARD_POLL_MS)
          .count())};
  if (key_pressed != PICO_ERROR_TIMEOUT) {
    if (key_pressed == 'q') {
      return UserInput::QUIT;
    }
    if (change_snake_direction(key_pressed)) {
      user_has_input = UserInput::CHANGE_DIRECTION;
    }
  }
#endif

  return user_has_input;
}

} // namespace

/* ==================================================================== */
/*    __  __       _         ____            _                          */
/*   |  \/  | __ _(_)_ __   | __ ) _   _ ___(_)_ __   ___  ___ ___      */
/*   | |\/| |/ _` | | '_ \  |  _ \| | | / __| | '_ \ / _ \/ __/ __|     */
/*   | |  | | (_| | | | | | | |_) | |_| \__ \ | | | |  __/\__ \__ \     */
/*   |_|  |_|\__,_|_|_| |_| |____/ \__,_|___/_|_| |_|\___||___/___/     */
/*                                                                      */
/*                                                  figlet -f standard  */
/* ==================================================================== */
namespace snake {

void run() {
  bool user_desires_play{true};

  init_the_screen();

  const GridLocation SNAKE_START{
      .x = static_cast<grid_t>(g_tile_grid.grid_width >> 1),
      .y = static_cast<grid_t>(g_tile_grid.grid_height)};
  const Direction SNAKE_DIR{Direction::UP};

  uint8_t growing{2};
  absolute_time_t last_time{get_absolute_time()};
  /* game loop! */
  while (user_desires_play) {
    int8_t lives{3};
    init_level(snake::levels[0]);
    init_snake(SNAKE_START, SNAKE_DIR);
    init_apples(); /* just places an apple somewhere */
    update_lives_on_screen(lives);
    draw_this_level(g_level.lvl);
    // draw_this_level(snake::levels[0]);
    bool initial_tick{true};
    while (lives > 0) {
      const auto now{get_absolute_time()};

      const auto user_input = process_user_input();

      if (user_input == UserInput::QUIT) {
        user_desires_play = false;
        break;
      }

      /* update screen with current game state */
      if (absolute_time_diff_us(last_time, now) > GAME_TICK_US ||
          user_input == UserInput::CHANGE_DIRECTION) {
        last_time = now;

        update_snake_state(growing > 0);

        if (growing > 0) {
          --growing;
        }

        /* handle collision cases */
        Collision collision{check_for_collisions()};
        /* initial_tick is a hack. We nullify collisions with the border when
         * the snake first enters the play area */
        if (initial_tick) {
          initial_tick = false;
          if (collision == Collision::BORDER) {
            collision = Collision::NONE;
          }
        }
        switch (collision) {
        case Collision::APPLE:
          remove_apple(g_collided_apple);
          growing += APPLE_GROWTH_TICKS;
          break;
        case Collision::BORDER:
        case Collision::SNAKE:
          growing = 2;
          cleanup_the_body();
          --lives;
          update_lives_on_screen(lives);
          init_snake(SNAKE_START, SNAKE_DIR);
          initial_tick = true;
          continue;
        case Collision::NONE:
          /* nothing to do */
          break;
        }

        update_borders(move_point(SNAKE_START, SNAKE_DIR));
        draw_snake();

        if (get_number_of_apples() == 0) {
          g_level.exit_is_open = true;
        }

        if (collision == Collision::EXIT) {
          growing = 2;
          cleanup_the_body();
          lives = 0;
          init_snake(SNAKE_START, SNAKE_DIR);
          initial_tick = true;
        }
      }
    }
  }
}
} // namespace snake
