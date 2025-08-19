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

#define USE_BGRID_OPTIMIZATION
namespace {

using snake::Direction;
using snake::grid_t;
using snake::pix_t;

/* Some game configurations */
constexpr std::chrono::milliseconds KEYBOARD_POLL_MS{
    1}; /* basically how often we poll for input */
constexpr auto GAME_TICK_US{300000U};
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

[[nodiscard]] constexpr bool operator==(snake::GridLocation lhs,
                                        snake::GridLocation rhs) noexcept {
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
  snake::GridLocation head;
  Direction head_dir;
  embp::circular_array<Direction, 255U> body_vec;
};

struct LevelState {
  bool exit_is_open;
  snake::GridLocation exit;
  snake::Level lvl;
};

TileGridCfg g_tile_grid;
SnakeState g_snake_state;
LevelState g_level;
uint32_t g_collided_apple;
embp::variable_array<snake::GridLocation, NUMBER_OF_APPLES> g_apple_locations;
std::array<uint32_t, snake::PLAY_SIZE>
    g_bgrid; /* bgrid = border grid, I guess */

[[nodiscard]] ScreenLocation to_pixel_xy(snake::GridLocation grid_xy) noexcept {
  /* TODO consider adding range checks here?  Or should it go somewhere else? */
  return {.x = static_cast<pix_t>(grid_xy.x * g_tile_grid.xdimension.scale +
                                  g_tile_grid.xdimension.off),
          .y = static_cast<pix_t>(grid_xy.y * g_tile_grid.ydimension.scale +
                                  g_tile_grid.ydimension.off)};
}

/** @brief Advances a snake::GridLocation in a given Direction
 *
 * @param point Grid Point
 * @param direction Up, Down, Left, or Right
 *
 * @return A new snake::GridLocation, advanced in the specified direction.
 */
[[nodiscard]] constexpr snake::GridLocation
move_point(snake::GridLocation point, Direction direction) noexcept {
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
void clear_screen_grid() noexcept {
  for (grid_t yy = 1; yy < g_tile_grid.grid_height - 1; ++yy) {
    for (grid_t xx = 1; xx < g_tile_grid.grid_width - 1; ++xx) {
      const auto [pixx, pixy]{to_pixel_xy({.x = xx, .y = yy})};
      screen::draw_tile(pixx, pixy, snake::BackgroundTile);
    }
  }
}
void clear_border_grid() noexcept {
  for (auto &c : g_bgrid) {
    c = uint32_t{};
  }
}

void set_border_grid_point(grid_t row, grid_t col) noexcept {
  /* TODO well, this is unfortunate.
     row and col are 1-indexed, since I encoded everything
     for the 33x33 sized screen when defining the levels.
     However, the render logic is 0-indexed, and as such the bgrid is 31x31 so
     each row neatly fits in 4 bytes.
     I guess I'll just hack in the corrections at the point of call?
   */
  g_bgrid[row] |= (1 << col);
}
[[nodiscard]] constexpr uint8_t
get_border_grid_point(const grid_t row, const grid_t col) noexcept {
  return static_cast<uint8_t>((g_bgrid[row] >> col) & 0b1);
}

void init_level(snake::Level lvl) noexcept {
  g_level.exit_is_open = false;
  g_level.exit = {.x = static_cast<grid_t>(g_tile_grid.grid_width >> 1),
                  .y = 0};
  g_level.lvl = lvl;
  clear_screen_grid();
  clear_border_grid();
}

[[nodiscard]] constexpr uint8_t encode_four_neighbours(grid_t row,
                                                       grid_t col) noexcept {

  constexpr grid_t LIMIT{snake::PLAY_SIZE - 1};

  const uint8_t right{col == LIMIT ? uint8_t{1U}
                                   : get_border_grid_point(row, col + 1)};
  const uint8_t left{col == 0 ? uint8_t{1U}
                              : get_border_grid_point(row, col - 1)};
  const uint8_t top{row == 0 ? uint8_t{1U}
                             : get_border_grid_point(row - 1, col)};
  const uint8_t bottom{row == LIMIT ? uint8_t{1U}
                                    : get_border_grid_point(row + 1, col)};

  /* from the comments in snake_tiles_constexpr.hpp:
   *     trbl
   *  BT_0000  :  just c
   */
  return ((top << 3) | (right << 2) | (bottom << 1) | left) & 0b1111;
}

void render_the_level() noexcept {
#ifdef USE_BGRID_OPTIMIZATION
  for (grid_t gridrow = 0; gridrow < std::size(g_bgrid); ++gridrow) {
    for (grid_t gridcol = 0; gridcol < snake::PLAY_SIZE; ++gridcol) {
      if (!get_border_grid_point(gridrow, gridcol)) {
        continue;
      }
      const auto bcode{encode_four_neighbours(gridrow, gridcol)};
      const auto [pixx,
                  pixy]{to_pixel_xy({.x = static_cast<grid_t>(gridcol + 1U),
                                     .y = static_cast<grid_t>(gridrow + 1U)})};
      screen::draw_tile(pixx, pixy, snake::BorderTiles[bcode]);
    }
  }
#endif
}

void draw_structure(snake::Point point) {
  set_border_grid_point(point.y - 1, point.x - 1);
}

void draw_structure(snake::Point point, const screen::Tile &tile) {
  const auto [pixx, pixy]{to_pixel_xy({.x = point.x, .y = point.y})};
  screen::draw_tile(pixx, pixy, tile);
}

void draw_structure(snake::StraightLine line) {
  const snake::Direction dir{line.dir};
  snake::GridLocation start{.x = line.xs, .y = line.ys};

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
    set_border_grid_point(start.y - 1, start.x - 1);
    --count;
    start.x += xinc;
    start.y += yinc;
  }
}
void draw_structure(snake::StraightLine line, const screen::Tile &tile) {
  const snake::Direction dir{line.dir};
  snake::GridLocation start{.x = line.xs, .y = line.ys};

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

void draw_structure(snake::Rectangle rect) {
  for (grid_t yy = rect.top; yy <= rect.bottom; ++yy) {
    for (grid_t xx = rect.left; xx <= rect.right; ++xx) {
      set_border_grid_point(yy - 1, xx - 1);
    }
  }
}
void draw_structure(snake::Rectangle rect, const screen::Tile &tile) {
  for (grid_t yy = rect.top; yy <= rect.bottom; ++yy) {
    for (grid_t xx = rect.left; xx <= rect.right; ++xx) {
      const auto [pixx, pixy]{to_pixel_xy({.x = xx, .y = yy})};
      screen::draw_tile(pixx, pixy, tile);
    }
  }
}

void draw_points(const uint8_t *p_data, uint32_t len) {
  static constexpr auto ENCODED_LENGTH{std::size(snake::encode_point(42, 42))};
  auto p_point_data{p_data};
  for (uint32_t ii = 0; ii < len; ++ii) {
#ifdef USE_BGRID_OPTIMIZATION
    draw_structure(snake::decode_point(p_point_data));
#else
    draw_structure(snake::decode_point(p_point_data), snake::BorderTiles[0]);
#endif
    std::advance(p_point_data, ENCODED_LENGTH);
  }
}
void draw_straight_lines(const uint8_t *p_data, uint32_t len) {
  static constexpr auto ENCODED_LENGTH{std::size(
      snake::encode_straight_line(42, 42, snake::Direction::DOWN, 0))};
  auto p_line_data{p_data};
  for (uint32_t ii = 0; ii < len; ++ii) {
#ifdef USE_BGRID_OPTIMIZATION
    draw_structure(snake::decode_straight_line(p_line_data));
#else
    draw_structure(snake::decode_straight_line(p_line_data),
                   snake::BorderTiles[0]);
#endif
    std::advance(p_line_data, ENCODED_LENGTH);
  }
}
void draw_rectangles(const uint8_t *p_data, uint32_t len) {
  static constexpr auto ENCODED_LENGTH{
      std::size(snake::encode_rectangle(42, 42, 43, 43))};
  for (uint32_t ii = 0; ii < len; ++ii) {
#ifdef USE_BGRID_OPTIMIZATION
    draw_structure(snake::decode_rectangle(p_data));
#else
    draw_structure(snake::decode_rectangle(p_data), snake::BorderTiles[0]);
#endif
    std::advance(p_data, ENCODED_LENGTH);
  }
}

void draw_this_level(snake::Level lvl) noexcept {
  const auto *p_structure{lvl.data};

  for (uint32_t ii = 0; ii < lvl.len; ++ii) {
    switch (p_structure[ii].type) {
    case snake::StructureType::STRAIGHT_LINE:
      draw_straight_lines(p_structure[ii].data, p_structure[ii].len);
      break;
    case snake::StructureType::POINT:
      draw_points(p_structure[ii].data, p_structure[ii].len);
      break;
    case snake::StructureType::RECT:
      draw_rectangles(p_structure[ii].data, p_structure[ii].len);
      break;
    default:
      /* TODO not yet implemented */
      break;
    }
  }

  render_the_level();
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
void init_snake(snake::GridLocation start, Direction dir) {
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
  /*
   * Previous direction is towards the head, next is towards the tail.
   * Given that the snake tiles are non-symmetric, we need to worry
   * about which end is the head and which end is the tail.
   *
   * We can do this by implementing the following lookup table:
   *
   *    Previous is row
   *    Next is column
   *
   *        | UP  | RIGHT | DOWN | LEFT |
   *        +-----|-------|------|------+
   *    UP  | DWN | DWRHT |  X   | DWLF |
   *  RIGHT | LUP |  LFT  | LFDW |  X   |
   *   DOWN |  X  | UPRHT |  UP  | UPLF |
   *   LEFT | RUP |   X   | RTDW | RHT  |
   */

  /* clang-format off */
  constexpr std::array<snake::SnakeBodyPart, 16> LUT{
    snake::SnakeBodyPart::BODY_DOWN, snake::SnakeBodyPart::BODY_DOWNRIGHT, snake::SnakeBodyPart::BODY_DOWN, snake::SnakeBodyPart::BODY_DOWNLEFT,
    snake::SnakeBodyPart::BODY_LEFTUP, snake::SnakeBodyPart::BODY_LEFT, snake::SnakeBodyPart::BODY_LEFTDOWN, snake::SnakeBodyPart::BODY_RIGHT,
    snake::SnakeBodyPart::BODY_UP, snake::SnakeBodyPart::BODY_UPRIGHT, snake::SnakeBodyPart::BODY_UP, snake::SnakeBodyPart::BODY_UPLEFT,
    snake::SnakeBodyPart::BODY_RIGHTUP, snake::SnakeBodyPart::BODY_RIGHT, snake::SnakeBodyPart::BODY_RIGHTDOWN, snake::SnakeBodyPart::BODY_RIGHT
  };
  /* clang-format on */

  const auto idx{static_cast<uint8_t>(previous) * 4 +
                 static_cast<uint8_t>(next)};

  return snake::to_snake_tile(LUT[idx]);
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

void cleanup_the_body() {
  auto head{g_snake_state.head};
  /* iterate through the body_vec to draw the rest */
  auto itr{g_snake_state.body_vec.begin()};
  for (const auto dir : g_snake_state.body_vec) {
    head = move_point(head, dir);
    const auto [pixx, pixy]{to_pixel_xy(head)};
    screen::draw_tile(pixx, pixy, snake::BackgroundTile);
  }
}

void draw_snake() {
  /* for right now, we'll just draw green squares */
  impl_draw_head(determine_snake_head_tile(g_snake_state.head_dir));
  impl_draw_the_snake_body();
}

void run_cleanup_animation() noexcept {
  static constexpr auto ANIMATION_TICK_US{GAME_TICK_US >> 1};
  absolute_time_t last_time{get_absolute_time()};
  while (!g_snake_state.body_vec.empty()) {
    absolute_time_t now{get_absolute_time()};
    if (absolute_time_diff_us(last_time, now) > ANIMATION_TICK_US) {
      last_time = now;
      clear_snake_tail();
      g_snake_state.body_vec.pop_back();
      draw_snake();
    }
  }
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
  constexpr auto BODYTILE{snake::to_snake_tile(snake::SnakeBodyPart::BODY_UP)};
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

bool configure_tile_grid() noexcept {

  const screen::Dimensions display_dims{screen::get_virtual_screen_size()};

  /*
   * Play area must be square.  The grid must be 33x33.
   */
  static constexpr auto grid_width{33};
  static constexpr auto grid_height{33};

  const auto screen_pix_y_off{display_dims.height - display_dims.width};
  const auto screen_pix_height{display_dims.height - screen_pix_y_off};

  const auto grid_scale{snake::BorderTiles[0].side_length};
  const auto grid_xoff{(display_dims.width - grid_scale * grid_width) >> 1};
  const auto grid_yoff{(screen_pix_height - grid_scale * grid_height) >> 1};

  if (grid_scale * grid_width > display_dims.width ||
      grid_scale * grid_height > display_dims.height) {
    return false;
  }

  g_tile_grid.grid_width = grid_width;
  g_tile_grid.grid_height = grid_height;
  g_tile_grid.xdimension.scale = grid_scale;
  g_tile_grid.xdimension.off = grid_xoff;
  g_tile_grid.ydimension.scale = grid_scale;
  g_tile_grid.ydimension.off = screen_pix_y_off + grid_yoff;

  return g_tile_grid.grid_width == 33 && g_tile_grid.grid_height == 33;
}

void draw_border() {
  /* border tiles all round the perimeter */

  /* top and bottom borders */
  {
    static constexpr auto LR_CODE{0b0101};
    grid_t gy = 0;
    for (grid_t gx = 1; gx < g_tile_grid.grid_width - 1; ++gx) {
      const auto [xx, yy]{to_pixel_xy({.x = gx, .y = gy})};
      screen::draw_tile(xx, yy, snake::BorderTiles[LR_CODE]);
    }
    gy = g_tile_grid.grid_height - 1;
    for (grid_t gx = 1; gx < g_tile_grid.grid_width - 1; ++gx) {
      const auto [xx, yy]{to_pixel_xy({.x = gx, .y = gy})};
      screen::draw_tile(xx, yy, snake::BorderTiles[LR_CODE]);
    }
  }

  /* left and right borders */
  {
    static constexpr auto TB_CODE{0b1010};
    grid_t gx = 0;
    for (grid_t gy = 1; gy < g_tile_grid.grid_height - 1; ++gy) {
      const auto [xx, yy]{to_pixel_xy({.x = gx, .y = gy})};
      screen::draw_tile(xx, yy, snake::BorderTiles[TB_CODE]);
    }
    gx = g_tile_grid.grid_width - 1;
    for (grid_t gy = 1; gy < g_tile_grid.grid_height - 1; ++gy) {
      const auto [xx, yy]{to_pixel_xy({.x = gx, .y = gy})};
      screen::draw_tile(xx, yy, snake::BorderTiles[TB_CODE]);
    }
  }

  /* the corners */
  static constexpr auto TL_CODE{0b1001};
  static constexpr auto TR_CODE{0b1100};
  static constexpr auto BL_CODE{0b0011};
  static constexpr auto BR_CODE{0b0110};
  {
    /* bottom right has connectivity on top and left */
    const auto [xx, yy]{to_pixel_xy({.x = 32, .y = 32})};
    screen::draw_tile(xx, yy, snake::BorderTiles[TL_CODE]);
  }
  {
    /* bottom left has connectivity on top and right */
    const auto [xx, yy]{to_pixel_xy({.x = 0, .y = 32})};
    screen::draw_tile(xx, yy, snake::BorderTiles[TR_CODE]);
  }
  {
    /* top right has connectivity on bottom and left */
    const auto [xx, yy]{to_pixel_xy({.x = 32, .y = 0})};
    screen::draw_tile(xx, yy, snake::BorderTiles[BL_CODE]);
  }
  {
    /* top left has connectivity on bottom and right */
    const auto [xx, yy]{to_pixel_xy({.x = 0, .y = 0})};
    screen::draw_tile(xx, yy, snake::BorderTiles[BR_CODE]);
  }
}

void update_borders(snake::GridLocation loc) noexcept {
  static constexpr auto LR_CODE{0b0101};
  /* Why the +1?  Because my bugfix for the entry bug was to start the snake
   * head one row above the border.  Since the 'loc' we are given is the snake
   * head, we account for this off-by-one behaviour here. */
  const auto [xx, yy]{to_pixel_xy({.x = loc.x, .y = loc.y + 1})};
  screen::draw_tile(xx, yy, snake::BorderTiles[LR_CODE]);

  if (g_level.exit_is_open) {
    const auto [xx, yy]{to_pixel_xy(g_level.exit)};
    screen::draw_tile(xx, yy, snake::BackgroundTile);
  } else {
    static constexpr auto LR_CODE{0b0101};
    const auto [xx, yy]{to_pixel_xy(g_level.exit)};
    screen::draw_tile(xx, yy, snake::BorderTiles[LR_CODE]);
  }
}

bool init_the_screen() noexcept {
  /* introspect, then configure the tile grid */
  if (!configure_tile_grid()) {
    return false;
  }

  /* we are a color application */
  screen::set_format(snake::TILE_FORMAT);
  screen::init_clut(snake::Palette.data(), snake::Palette.size());

  /* we use a black background */
  screen::clear_screen();

  /* we setup a blue border */
  draw_border();

  return true;
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

[[nodiscard]] bool
check_for_apple_collision(snake::GridLocation point,
                          uint32_t &collided_apple_index) noexcept {
  uint32_t idx{0};
  for (const auto apple : g_apple_locations) {
    if (apple == point) {
      collided_apple_index = idx;
      return true;
    }
    ++idx;
  }
  return false;
}

[[nodiscard]] bool
check_for_itself_collision(snake::GridLocation point) noexcept {
  /* if the head is equal to any of it's body, we have a collision */
  const auto head{point};
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
check_for_point_collisions(snake::GridLocation point, const uint8_t *p_data,
                           uint32_t len) noexcept {
  for (uint32_t ii = 0; ii < len; ++ii) {
    const snake::Point pointdef{snake::decode_point(p_data)};
    const snake::GridLocation border_point{.x = pointdef.x, .y = pointdef.y};
    if (point == border_point) {
      return true;
    }
    std::advance(p_data, 2); /* TODO 2 is a magic number... */
  }
  return false;
}

[[nodiscard]] constexpr bool
check_for_rect_collisions(snake::GridLocation point, const uint8_t *p_data,
                          uint32_t len) noexcept {

  for (uint32_t ii = 0; ii < len; ++ii) {
    const snake::Rectangle rectdef{snake::decode_rectangle(p_data)};
    if (snake::check_intersects(point, rectdef)) {
      return true;
    }
    std::advance(p_data, 3); /* TODO 3 is a magic number */
  }
  return false;
}

[[nodiscard]] constexpr bool check_for_straight_line_collisions(
    snake::GridLocation point, const uint8_t *p_data, uint32_t len) noexcept {
  for (uint32_t ii = 0; ii < len; ++ii) {
    const snake::StraightLine linedef{snake::decode_straight_line(p_data)};
    if (snake::check_intersects(point, linedef)) {
      return true;
    }
    std::advance(p_data, 3); /* TODO 3 is a magic number... */
  }
  return false;
}

[[nodiscard]] constexpr bool
check_for_level_collisions(snake::GridLocation point,
                           snake::Level lvl) noexcept {
  const auto *p_structure{lvl.data};
  for (uint32_t ii = 0; ii < lvl.len; ++ii) {
    switch (p_structure[ii].type) {
    case snake::StructureType::STRAIGHT_LINE:
      if (check_for_straight_line_collisions(point, p_structure[ii].data,
                                             p_structure[ii].len)) {
        return true;
      }
      break;
    case snake::StructureType::POINT:
      if (check_for_point_collisions(point, p_structure[ii].data,
                                     p_structure[ii].len)) {
        return true;
      }
      break;
    case snake::StructureType::RECT:
      if (check_for_rect_collisions(point, p_structure[ii].data,
                                    p_structure[ii].len)) {
        return true;
      }
      break;
    default:
      /* TODO implement the rest... */
      break;
    }
  }
  return false;
}

[[nodiscard]] Collision
check_for_collisions(snake::GridLocation point) noexcept {
  /* If head is on the exit, and the door is open, then make like a tree and get
   * outta here. */
  if (g_level.exit_is_open && point == g_level.exit) {
    return Collision::EXIT;
  }

  /* if head is on the border, whoops! Game over. */
  if (point.x < 1 || point.x > g_tile_grid.grid_width - 2 || point.y < 1 ||
      point.y > g_tile_grid.grid_height - 2 ||
      check_for_level_collisions(point, g_level.lvl)) {
    return Collision::BORDER;
  }

  /* if the head is on itself, whoa!  No cannabilism allowed. */
  if (check_for_itself_collision(point)) {
    return Collision::SNAKE;
  }
  /* if head is on an apple, yum! Remove the apple. */
  if (check_for_apple_collision(point, g_collided_apple)) {
    return Collision::APPLE;
  }

  return Collision::NONE;
}

/* ===================================================== */
/*      _                _         ____           _      */
/*     / \   _ __  _ __ | | ___   / ___|__ _ _ __| |_    */
/*    / _ \ | '_ \| '_ \| |/ _ \ | |   / _` | '__| __|   */
/*   / ___ \| |_) | |_) | |  __/ | |__| (_| | |  | |_    */
/*  /_/   \_\ .__/| .__/|_|\___|  \____\__,_|_|   \__|   */
/*          |_|   |_|                                    */
/* ===================================================== */

void init_apples() noexcept {
  /* if there are already apples, i.e. the level restarted, clear the old ones
   */
  for (const auto apple : g_apple_locations) {
    const auto [xx, yy]{to_pixel_xy(apple)};
    screen::draw_tile(xx, yy, snake::BackgroundTile);
  }

  /* Naive impl, but whatever... just places an apple somewhere
   *
   * Pick a random location in the play area, then check for collision,
   * If we collide, just try again.
   *
   */
  [[maybe_unused]] uint32_t prev_apple;
  g_apple_locations.clear();
  for (uint32_t idx = 0; idx < NUMBER_OF_APPLES; ++idx) {
    while (true) {
      const uint32_t seed{get_rand_32()};
      /* play area is always 31x31... this makes the modulo math easy... what a
       * COINCIDENCE
       *
       * Anyways, valid x,y locations for apples are
       *   in the range [1,31]
       *   not under a wall
       */
      static constexpr auto MASK{0b11111U};
      const grid_t xloc{static_cast<grid_t>((seed & MASK) + 1)};
      const grid_t yloc{static_cast<grid_t>(((seed >> 16) & MASK) + 1)};
      const snake::GridLocation apple{.x = xloc, .y = yloc};
      if (xloc > 31 || yloc > 31 ||
          check_for_level_collisions(apple, g_level.lvl) ||
          check_for_apple_collision(apple, prev_apple)) {
        continue;
      }
      g_apple_locations.push_back(apple);
      break;
    }
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

/* ======================================================= */
/*     _   _                 ___                   _       */
/*    | | | |___  ___ _ __  |_ _|_ __  _ __  _   _| |_     */
/*    | | | / __|/ _ \ '__|  | || '_ \| '_ \| | | | __|    */
/*    | |_| \__ \  __/ |     | || | | | |_) | |_| | |_     */
/*     \___/|___/\___|_|    |___|_| |_| .__/ \__,_|\__|    */
/*                                    |_|                  */
/* ======================================================= */
enum struct UserInput { QUIT, PLAY, CHANGE_DIRECTION, NEXT_LEVEL };

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
    if (key_pressed == 'n') {
      return UserInput::NEXT_LEVEL;
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

  if (!init_the_screen()) {
    printf("couldn't init screen\n");
    return;
  }

  const snake::GridLocation SNAKE_START{
      .x = static_cast<grid_t>(g_tile_grid.grid_width >> 1),
      .y = static_cast<grid_t>(g_tile_grid.grid_height - 1)};
  static constexpr Direction SNAKE_DIR{Direction::UP};

  static constexpr uint8_t GROWING_START{4};
  uint32_t lvl_idx{};
  uint8_t growing{GROWING_START + lvl_idx};
  absolute_time_t last_time{get_absolute_time()};
  int8_t lives{3};
  /* game loop! */
  while (user_desires_play) {
    init_level(snake::levels[lvl_idx]);
    init_snake(SNAKE_START, SNAKE_DIR);
    init_apples(); /* just places an apple somewhere */
    update_lives_on_screen(lives);
    draw_this_level(g_level.lvl);
    bool initial_tick{true};
    bool level_is_active{true};
    while (level_is_active) {
      const auto now{get_absolute_time()};

      const auto user_input = process_user_input();

      if (user_input == UserInput::QUIT) {
        user_desires_play = false;
        break;
      }
      if (user_input == UserInput::NEXT_LEVEL) {
        lvl_idx = lvl_idx == snake::levels.size() - 1 ? 0 : lvl_idx + 1;
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
        Collision collision{check_for_collisions(g_snake_state.head)};
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
          growing = GROWING_START + lvl_idx;
          cleanup_the_body();
          --lives;
          update_lives_on_screen(lives);
          init_snake(SNAKE_START, SNAKE_DIR);
          initial_tick = true;
          level_is_active = lives > 0;
          /* TODO this is only for development, and should trigger a Game Over,
           * instead */
          if (!level_is_active) {
            lives = 3;
          }
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
          run_cleanup_animation();
          growing = GROWING_START + lvl_idx;
          cleanup_the_body();
          level_is_active = false;
          init_snake(SNAKE_START, SNAKE_DIR);
          initial_tick = true;
          lvl_idx = lvl_idx == snake::levels.size() - 1 ? 0 : lvl_idx + 1;
        }
      }
    }
  }
}
} // namespace snake
