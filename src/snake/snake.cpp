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
#include "screen/glyphs/letters.hpp"
#include "screen/screen.hpp"

#include "gamepad/gamepad.hpp"

#include "embp/circular_array.hpp"

#include "../screen_utils.hpp"
#include "snake_common.hpp"
#include "snake_levels_constexpr.hpp"
#include "snake_tiles_constexpr.hpp"

#define SNAKE_USE_GAMEPAD
#define USE_BGRID_OPTIMIZATION

// #define PRINT_DEBUG_MSG

namespace {

using snake::Direction;
using snake::grid_t;
using snake::pix_t;

/* Some game configurations */
constexpr std::chrono::milliseconds KEYBOARD_POLL_MS{
    1}; /* basically how often we poll for input */
constexpr auto GAME_TICK_US{250000U};
constexpr auto APPLE_GROWTH_TICKS{3U};
constexpr auto NUMBER_OF_APPLES{10U};
constexpr uint8_t INITIAL_TICK_HACK_TUNING{1};

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

/* apples only live on the grid, which is 31x31
 * which means, only need 5 bits per axis to locate them
 * we'll use one of our extra bits to mark if the apple is
 * green or not
 */
struct Apple {
  grid_t x;
  grid_t y : 7;
  bool is_green : 1;
};
static_assert(sizeof(Apple) == 2);

[[nodiscard]] constexpr bool operator==(snake::GridLocation lhs,
                                        Apple rhs) noexcept {
  return lhs.x == rhs.x && lhs.y == rhs.y;
}
[[nodiscard]] constexpr bool operator==(Apple lhs,
                                        snake::GridLocation rhs) noexcept {
  return rhs == lhs;
}

TileGridCfg g_tile_grid;
SnakeState g_snake_state;
LevelState g_level;
uint32_t g_collided_apple;
embp::variable_array<Apple, NUMBER_OF_APPLES * NUMBER_OF_APPLES>
    g_apple_locations;
std::array<uint32_t, snake::PLAY_SIZE>
    g_bgrid; /* bgrid = border grid, I guess */

[[nodiscard]] ScreenLocation to_pixel_xy(snake::GridLocation grid_xy) noexcept {
  /* TODO consider adding range checks here?  Or should it go somewhere else? */
  return {.x = static_cast<pix_t>(grid_xy.x * g_tile_grid.xdimension.scale +
                                  g_tile_grid.xdimension.off),
          .y = static_cast<pix_t>(grid_xy.y * g_tile_grid.ydimension.scale +
                                  g_tile_grid.ydimension.off)};
}

void draw_grid_tile(grid_t x, grid_t y, const screen::Tile &tile) {
  const auto [pixx, pixy]{to_pixel_xy({.x = x, .y = y})};
  screen::draw_tile(pixx, pixy, tile);
}

/** @brief convert 1bpp letter tile into double-sized, 4bpp versions
 */
template <size_t N>
constexpr void copy_tile_to_4bpp_buffer_and_double_in_size(
    std::array<uint8_t, N> &tile_4bpp, const screen::Tile &tile_1bpp) noexcept {
  const auto *p_in{tile_1bpp.data};
  auto *p_out{std::data(tile_4bpp)};

  for (uint32_t yy = 0; yy < glyphs::tile::height() * 2; ++yy) {
    auto inrow{p_in[yy >> 1]};
    for (uint32_t xx = 0; xx < glyphs::tile::width() * 2; xx += 2) {
      const uint32_t outpixloc{yy * glyphs::tile::width() * 2 + xx};
      auto &out{p_out[outpixloc >> 1]};

      out = inrow & 0b1 ? snake::WHITE << 4 | snake::WHITE
                        : snake::BLACK << 4 | snake::BLACK;
      inrow >>= 1;
    }
  }
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
      draw_grid_tile(xx, yy, snake::BackgroundTile);
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
      draw_grid_tile(static_cast<grid_t>(gridcol + 1U),
                     static_cast<grid_t>(gridrow + 1U),
                     snake::BorderTiles[bcode]);
    }
  }
#endif
}

void draw_structure(snake::Point point) {
  set_border_grid_point(point.y - 1, point.x - 1);
}

void draw_structure(snake::Point point, const screen::Tile &tile) {
  draw_grid_tile(point.x, point.y, tile);
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
    draw_grid_tile(start.x, start.y, tile);
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
      draw_grid_tile(xx, yy, tile);
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
  draw_grid_tile(head.x, head.y, snake::BackgroundTile);
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
  draw_grid_tile(head.x, head.y, tile);
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
    draw_grid_tile(head.x, head.y, tile);
    itr++;
  }

  /* draw the tail */
  const auto dir{*itr};
  const auto tile{determine_snake_tail_tile(dir)};
  head = move_point(head, dir);
  draw_grid_tile(head.x, head.y, tile);
}

void cleanup_the_body() {
  auto head{g_snake_state.head};
  /* iterate through the body_vec to draw the rest */
  auto itr{g_snake_state.body_vec.begin()};
  for (const auto dir : g_snake_state.body_vec) {
    head = move_point(head, dir);
    draw_grid_tile(head.x, head.y, snake::BackgroundTile);
  }
}

void draw_snake() {
  /* for right now, we'll just draw green squares */
  impl_draw_head(determine_snake_head_tile(g_snake_state.head_dir));
  impl_draw_the_snake_body();
}

void run_cleanup_animation() noexcept {
  static constexpr auto ANIMATION_TICK_US{GAME_TICK_US >> 2};
  static constexpr auto BODYTILE{
      snake::to_snake_tile(snake::SnakeBodyPart::BODY_UP)};

  /* play the animation */
  absolute_time_t last_time{get_absolute_time()};
  /* spin for a game tick */
  sleep_us(GAME_TICK_US);
  while (!g_snake_state.body_vec.empty()) {
    absolute_time_t now{get_absolute_time()};
    if (absolute_time_diff_us(last_time, now) > ANIMATION_TICK_US) {
      last_time = now;
      clear_snake_tail();
      g_snake_state.body_vec.pop_back();
      draw_snake();
      /* replace the head with a snake body running up */
      draw_grid_tile(g_snake_state.head.x, g_snake_state.head.y, BODYTILE);
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
constexpr uint32_t TIMER_HEIGHT_PIX{3};
constexpr uint32_t LIVES_HEIGHT_TILES{4};

struct TopPanelCfg {
  uint16_t row_start_lives;
  uint16_t col_start_lives;
  uint16_t col_inc_lives;
  uint16_t lives_height_tiles;

  uint16_t row_start_timer;
  uint16_t col_start_timer;
  uint16_t timer_col_length;

  uint16_t row_start_score;
  uint16_t col_start_score;
};
TopPanelCfg g_top_panel_cfg;

void init_top_panel_config(const TileGridCfg &grid_cfg) {
  static constexpr auto HEADTILE{
      snake::to_snake_tile(snake::SnakeBodyPart::HEAD_UP)};
  static constexpr auto BODYTILE{
      snake::to_snake_tile(snake::SnakeBodyPart::BODY_UP)};
  static constexpr auto TAILTILE{
      snake::to_snake_tile(snake::SnakeBodyPart::TAIL_DOWN)};
  static_assert(HEADTILE.side_length == BODYTILE.side_length);
  static_assert(HEADTILE.side_length == TAILTILE.side_length);
  static constexpr auto TILE_INC{HEADTILE.side_length};

  const screen::Dimensions dims{screen::get_virtual_screen_size()};
  /*

    hs = lives_height_tiles * TILE_INC
    ht = 5
    hm = g_tile_grid.ydimension.off

    hm = hs + ht + 3 * hg
    solve for hg:
    hg = ( hm - hs - ht ) / 3

    then,
    row_start_lives = hg
    row_start_timer = hs + 2*hg

   */
  const auto HEIGHT_LIVES{LIVES_HEIGHT_TILES * TILE_INC};
  const auto HEIGHT_GAPS{
      (grid_cfg.ydimension.off - HEIGHT_LIVES - TIMER_HEIGHT_PIX) / 3};
  const auto col_inc{(TILE_INC << 1) - 1};
  const auto col_start{TILE_INC >> 1};

  g_top_panel_cfg.row_start_lives = HEIGHT_GAPS;
  g_top_panel_cfg.col_start_lives = col_start;
  g_top_panel_cfg.col_inc_lives = col_inc;
  g_top_panel_cfg.lives_height_tiles = LIVES_HEIGHT_TILES;
  g_top_panel_cfg.row_start_timer = HEIGHT_LIVES + 2 * HEIGHT_GAPS;
  g_top_panel_cfg.col_start_timer = grid_cfg.xdimension.off * 2;
  g_top_panel_cfg.timer_col_length =
      grid_cfg.grid_width * grid_cfg.xdimension.scale -
      grid_cfg.xdimension.off * 2;

  g_top_panel_cfg.row_start_score =
      HEIGHT_GAPS + ((HEIGHT_LIVES - 2 * glyphs::tile::width()) >> 1);
  g_top_panel_cfg.col_start_score =
      dims.width - (grid_cfg.xdimension.off + 2 * 6 * glyphs::tile::width());
}

void update_lives_on_screen(uint8_t lives) noexcept {
  static constexpr int DRAW_LIMIT{7}; /* TODO this also needs tuning */
  static constexpr auto HEADTILE{
      snake::to_snake_tile(snake::SnakeBodyPart::HEAD_UP)};
  static constexpr auto BODYTILE{
      snake::to_snake_tile(snake::SnakeBodyPart::BODY_UP)};
  static constexpr auto TAILTILE{
      snake::to_snake_tile(snake::SnakeBodyPart::TAIL_DOWN)};
  static_assert(HEADTILE.side_length == BODYTILE.side_length);
  static_assert(HEADTILE.side_length == TAILTILE.side_length);
  static constexpr auto TILE_INC{HEADTILE.side_length};

  static uint8_t prev_lives{255};

  const auto row_start{g_top_panel_cfg.row_start_lives};
  const auto col_inc{g_top_panel_cfg.col_inc_lives};

  if (lives > 0) {
    --lives;
  }

  /* let's prevent redrawing stuff if we don't need to... */
  if (prev_lives == lives) {
    return;
  }
  prev_lives = lives;

  /* these are drawn outside of the grid, so we need to handle pixel placement
   * manually */
  auto col_start{g_top_panel_cfg.col_start_lives};
  for (int ii = 0; ii < DRAW_LIMIT; ++ii) {
    const auto col{col_start + ii * col_inc};
    for (int rs = 0; rs < g_top_panel_cfg.lives_height_tiles; ++rs) {
      screen::draw_tile(col, row_start + rs * TILE_INC, snake::BackgroundTile);
    }
  }
  const auto update_limit{lives > DRAW_LIMIT ? DRAW_LIMIT : lives};
  for (int ii = 0; ii < update_limit; ++ii) {
    screen::draw_tile(col_start, row_start, HEADTILE);
    for (int rs = 1; rs < g_top_panel_cfg.lives_height_tiles - 1; ++rs) {
      screen::draw_tile(col_start, row_start + rs * TILE_INC, BODYTILE);
    }
    screen::draw_tile(col_start,
                      row_start +
                          (g_top_panel_cfg.lives_height_tiles - 1) * TILE_INC,
                      TAILTILE);
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
  static constexpr auto LR_CODE{0b0101};
  static constexpr auto TB_CODE{0b1010};
  static constexpr auto TL_CODE{0b1001};
  static constexpr auto TR_CODE{0b1100};
  static constexpr auto BL_CODE{0b0011};
  static constexpr auto BR_CODE{0b0110};

  /* top and bottom borders */
  {
    grid_t gy = 0;
    for (grid_t gx = 1; gx < g_tile_grid.grid_width - 1; ++gx) {
      draw_grid_tile(gx, gy, snake::BorderTiles[LR_CODE]);
    }
    gy = g_tile_grid.grid_height - 1;
    for (grid_t gx = 1; gx < g_tile_grid.grid_width - 1; ++gx) {
      draw_grid_tile(gx, gy, snake::BorderTiles[LR_CODE]);
    }
  }

  /* left and right borders */
  {
    grid_t gx = 0;
    for (grid_t gy = 1; gy < g_tile_grid.grid_height - 1; ++gy) {
      draw_grid_tile(gx, gy, snake::BorderTiles[TB_CODE]);
    }
    gx = g_tile_grid.grid_width - 1;
    for (grid_t gy = 1; gy < g_tile_grid.grid_height - 1; ++gy) {
      draw_grid_tile(gx, gy, snake::BorderTiles[TB_CODE]);
    }
  }

  /* the corners */
  /* bottom right has connectivity on top and left */
  draw_grid_tile(32, 32, snake::BorderTiles[TL_CODE]);
  /* bottom left has connectivity on top and right */
  draw_grid_tile(0, 32, snake::BorderTiles[TR_CODE]);
  /* top right has connectivity on bottom and left */
  draw_grid_tile(32, 0, snake::BorderTiles[BL_CODE]);
  /* top left has connectivity on bottom and right */
  draw_grid_tile(0, 0, snake::BorderTiles[BR_CODE]);
}

void update_borders(snake::GridLocation loc) noexcept {
  static constexpr auto LR_CODE{0b0101};
  /* why the +1?  because I'm a hack... also, the fix for a render bug on the
   * snake's entry to the level was to start the snake head one row up.  we need
   * to compensate for this here.*/
  draw_grid_tile(loc.x, loc.y + 1, snake::BorderTiles[LR_CODE]);

  const auto &tile{g_level.exit_is_open ? snake::BackgroundTile
                                        : snake::BorderTiles[LR_CODE]};
  draw_grid_tile(g_level.exit.x, g_level.exit.y, tile);
}

bool init_the_screen() noexcept {
  /* introspect, then configure the tile grid */
  if (!configure_tile_grid()) {
    return false;
  }
  init_top_panel_config(g_tile_grid);

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

[[nodiscard]] constexpr bool
check_for_itself_collision_impl(snake::GridLocation point,
                                const SnakeState &snake) noexcept {
  /* if the head is equal to any of it's body, we have a collision */
  auto start{snake.head};
  for (const auto dir : snake.body_vec) {
    start = move_point(start, dir);
    if (start == point) {
      return true;
    }
  }
  return false;
}
[[nodiscard]] constexpr bool
check_for_itself_collision(snake::GridLocation point) noexcept {
  return check_for_itself_collision_impl(point, g_snake_state);
}

namespace constexpr_tests {
[[nodiscard]] constexpr bool test_itself_collision_impl() {
  bool result{true};
  SnakeState dut;
  /* move the snake a bunch of times */

  dut.head = snake::GridLocation{.x = 15, .y = 30};
  dut.head_dir = Direction::UP;
  dut.body_vec.clear();

  /* move left, up, right, up */
  auto &&update_snake{[&](Direction dir) {
    /* head always moves */
    dut.head = move_point(dut.head, dir);

    /* add to the length of the body, in the opposite direction */
    dut.body_vec.push_front(get_opposite(dir));
  }};

  update_snake(Direction::LEFT);
  result &= dut.head == snake::GridLocation{.x = 14, .y = 30};
  update_snake(Direction::UP);
  result &= dut.head == snake::GridLocation{.x = 14, .y = 29};
  update_snake(Direction::RIGHT);
  result &= dut.head == snake::GridLocation{.x = 15, .y = 29};
  update_snake(Direction::UP);
  result &= dut.head == snake::GridLocation{.x = 15, .y = 28};

  /* current snake state
   *      (x,y)
   * head (15,28)
   * body (15,29)
   * body (14,29)
   * body (14,30)
   * body (15,30)
   */

  /* now, we test for collisions with the body */
  result &= check_for_itself_collision_impl({.x = 15, .y = 30}, dut);
  result &= check_for_itself_collision_impl({.x = 14, .y = 30}, dut);
  result &= check_for_itself_collision_impl({.x = 14, .y = 29}, dut);
  result &= check_for_itself_collision_impl({.x = 15, .y = 29}, dut);

  /* the head shouldn't collide with itself with this current state */
  result &= (check_for_itself_collision_impl(dut.head, dut) == false);

  /* move the head so that it does collide, and check */
  update_snake(Direction::RIGHT);
  update_snake(Direction::DOWN);
  update_snake(Direction::LEFT);

  result &= check_for_itself_collision_impl(dut.head, dut);

  return result;
}
static_assert(test_itself_collision_impl());
} // namespace constexpr_tests

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
[[nodiscard]] Apple make_apple() noexcept {
  [[maybe_unused]] uint32_t prev_apple;
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
    const Apple apple{.x = xloc, .y = yloc, .is_green = false};
    /* check_for_collisions is optimized for the snake head.
     * however, when placing apples, we also need to check for the location of
     * the snake head.
     */
    if (Collision::NONE != check_for_collisions({.x = apple.x, .y = apple.y}) ||
        apple == g_snake_state.head) {
      continue;
    }
#if PRINT_DEBUG_MSG
    printf("new apple  { .x = %d, .y = %d }\n", apple.x, apple.y);
#endif
    return apple;
  }
}

void add_apples(uint32_t count) noexcept {
  const uint32_t room_left{g_apple_locations.capacity() -
                           g_apple_locations.size()};
  const uint32_t to_add{count < room_left ? count : room_left};
  for (uint32_t idx = 0; idx < to_add; ++idx) {
    g_apple_locations.push_back(make_apple());
  }
#if PRINT_DEBUG_MSG
  printf("**\nour new apple locations are\n");
  for (const auto &appl : g_apple_locations) {
    printf("  {.x=%d, .y=%d}\n", appl.x, appl.y);
  }
#endif
}

void draw_apple(const Apple &apple) noexcept {
  if (apple.is_green) {
    draw_grid_tile(apple.x, apple.y, snake::GreenAppleTile);
  } else {
    draw_grid_tile(apple.x, apple.y, snake::AppleTile);
  }
}

void draw_apples() noexcept {
  for (const auto apple : g_apple_locations) {
    draw_apple(apple);
  }
}

void init_apples() noexcept {
  /* if there are already apples, i.e. the level restarted, clear the old ones
   */
  for (const auto apple : g_apple_locations) {
    draw_grid_tile(apple.x, apple.y, snake::BackgroundTile);
  }

  /* Naive impl, but whatever... just places an apple somewhere
   *
   * Pick a random location in the play area, then check for collision,
   * If we collide, just try again.
   *
   */
  g_apple_locations.clear();
  for (uint32_t idx = 0; idx < NUMBER_OF_APPLES; ++idx) {
    g_apple_locations.push_back(make_apple());
  }

  draw_apples();
}

[[nodiscard]] uint32_t get_number_of_apples() noexcept {
  return g_apple_locations.size();
}

void remove_apple(uint32_t apple_idx) noexcept {
  const auto itr{std::next(g_apple_locations.begin(), apple_idx)};
  g_apple_locations.erase(itr);
}

/* there's a 1-in-MERP chance a red apple turn green */
void process_adding_green_apple() noexcept {
  const auto roll{get_rand_32()};

  if ((roll & 0b1111111) == 0) {
    /* pick the first apple that isn't red, and make it green */
    for (auto &apple : g_apple_locations) {
      if (!apple.is_green) {
        apple.is_green = true;
        draw_apple(apple);
        break;
      }
    }
  }
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
#ifdef SNAKE_USE_GAMEPAD
  sleep_ms(KEYBOARD_POLL_MS.count());
  const gamepad::five::State key_pressed{gamepad::five::get()};

  if (key_pressed.etc) {
    return UserInput::QUIT;
  }

  int user_key{0};
  if (key_pressed.up) {
    user_key = 'i';
  } else if (key_pressed.left) {
    user_key = 'j';
  } else if (key_pressed.down) {
    user_key = 'k';
  } else if (key_pressed.right) {
    user_key = 'l';
  } else {
    user_key = 0;
  }

  if (change_snake_direction(user_key)) {
    user_has_input = UserInput::CHANGE_DIRECTION;
  }
#else
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
#endif

  return user_has_input;
}

/* ==================================================================== *
                     _____ _
                    |_   _(_)_ __ ___   ___ _ __
                      | | | | '_ ` _ \ / _ \ '__|
                      | | | | | | | | |  __/ |
                      |_| |_|_| |_| |_|\___|_|

 * ==================================================================== */
/* 8 times the side length of the play area */
constexpr uint32_t TIMER_BIT_DEPTH{8};
constexpr int TIMER_START_VALUE{1 << TIMER_BIT_DEPTH}; /* aka, 32*8 = 256 */

int g_timer{TIMER_START_VALUE};

void reset_timer() noexcept { g_timer = TIMER_START_VALUE; }

void process_timer() noexcept {
  if (g_timer > 0) {
    --g_timer;
  } else {
    g_timer = 0;
  }
}

[[nodiscard]] bool check_timer_expired() noexcept { return g_timer == 0; }

void draw_timer() noexcept {
  /* now, here's where things get interesting
   *
   * it's rendered as a green line that slowly goes down
   * alternatively, it's a black line that slowly grows from the right side,
   * replacing the existing green bar.
   *
   * timer_pixel_width * g_timer / TIMER_START_VALUE == length of bar that is
   * green timer_pixel_width - length of bar that is green == length of bar
   * that is black
   *
   */

  const uint32_t row_start{g_top_panel_cfg.row_start_timer};
  const uint32_t row_stop{row_start + TIMER_HEIGHT_PIX};
  const uint32_t col_start{g_top_panel_cfg.col_start_timer};
  const uint32_t col_stop{col_start + g_top_panel_cfg.timer_col_length};

  const uint32_t green_length{
      (g_top_panel_cfg.timer_col_length * static_cast<uint32_t>(g_timer)) >>
      TIMER_BIT_DEPTH};
  const uint32_t black_length{g_top_panel_cfg.timer_col_length - green_length};

  static constexpr uint8_t timer_green_idx{snake::TIMEGRN};
  static constexpr uint8_t timer_black_idx{snake::BLACK};
  static constexpr screen::Tile black_pix{
      .side_length = 1, .format = snake::TILE_FORMAT, .data = &timer_black_idx};
  static constexpr screen::Tile green_pix{
      .side_length = 1, .format = snake::TILE_FORMAT, .data = &timer_green_idx};

  /* draw green first, then black */
  for (uint32_t yy = row_start; yy < row_stop; ++yy) {
    for (uint32_t xx = col_start; xx < col_start + green_length; ++xx) {
      screen::draw_tile(xx, yy, green_pix);
    }
  }
  for (uint32_t yy = row_start; yy < row_stop; ++yy) {
    for (uint32_t xx = col_start + green_length; xx < col_stop; ++xx) {
      screen::draw_tile(xx, yy, black_pix);
    }
  }
}

/* ==================================================================== *
                   ____
                  / ___|  ___ ___  _ __ ___
                  \___ \ / __/ _ \| '__/ _ \
                   ___) | (_| (_) | | |  __/
                  |____/ \___\___/|_|  \___|

 * ==================================================================== */
constexpr uint32_t EXTRA_LIFE_INTERVAL{50};
uint32_t g_score;
uint32_t g_next_life_threshold{EXTRA_LIFE_INTERVAL};

enum struct ScoreSource { RED_APPLE, GREEN_APPLE, TIMER };

[[nodiscard]] uint32_t determine_score(ScoreSource source) noexcept {
  switch (source) {
  case ScoreSource::RED_APPLE:
    return 1U;
  case ScoreSource::GREEN_APPLE:
    return (get_rand_32() & 0b1) + 2;
  case ScoreSource::TIMER:
    return ((10 * g_timer) >> TIMER_BIT_DEPTH) +
           3; /* TODO this needs some tuning */
  }
  return 0;
}
void reset_score() noexcept { g_score = 0; }

void increment_score(uint32_t value) noexcept { g_score += value; }

[[nodiscard]] uint32_t get_score() noexcept { return g_score; }

void draw_score() {

  /* convert integral score into 6 decimal digits */
  const auto digits{screen::bcd<6>(g_score)};

  /* convert the existing 1bpp number glyphs into double-sized 4bpp versions */
  std::array<uint8_t, 4 * 4 * glyphs::tile::height()> letter_4bpp_data{};
  const screen::Tile letter_tile{.side_length = glyphs::tile::width() * 2,
                                 .format = screen::Format::RGB565_LUT4,
                                 .data = std::data(letter_4bpp_data)};

  /* go from msd to lsd */
  const auto row_start{g_top_panel_cfg.row_start_score};
  auto col_start{g_top_panel_cfg.col_start_score};
  for (const auto digit : digits) {
    const auto tile_1bpp{
        glyphs::tile::decode_ascii(static_cast<char>(digit + 0x30))};
    copy_tile_to_4bpp_buffer_and_double_in_size(letter_4bpp_data, tile_1bpp);
    screen::draw_tile(col_start, row_start, letter_tile);
    col_start += letter_tile.side_length;
  }
}

void update_lives_based_on_score(int8_t &lives) noexcept {
  if (g_score >= g_next_life_threshold) {
    ++lives;
    g_next_life_threshold += EXTRA_LIFE_INTERVAL;
  }
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

  gamepad::five::init();

  const snake::GridLocation SNAKE_START{
      .x = static_cast<grid_t>(g_tile_grid.grid_width >> 1),
      .y = static_cast<grid_t>(g_tile_grid.grid_height - 1)};
  static constexpr Direction SNAKE_DIR{Direction::UP};

  static constexpr uint8_t GROWING_START{4};
  uint32_t lvl_idx{};
  uint32_t growing{GROWING_START + lvl_idx};
  absolute_time_t last_time{get_absolute_time()};
  int8_t lives{3};
  reset_score();
  /* game loop! */
  while (user_desires_play) {
    /* stuff that needs to happen on start of every new level */
    init_level(snake::levels[lvl_idx]);
    // draw_level_name(lvl_idx + 1);
    init_snake(SNAKE_START, SNAKE_DIR);
    init_apples(); /* just places an apple somewhere */
    update_lives_on_screen(lives);
    draw_this_level(g_level.lvl);
    reset_timer();
    draw_timer();
    draw_score();
    uint8_t initial_tick{INITIAL_TICK_HACK_TUNING};
    bool level_is_active{true};
    while (level_is_active) {
      const auto now{get_absolute_time()};

      /* process user input and take action immediately if required,
       *  (unless we are just starting...)
       */
      const auto user_input =
          !initial_tick ? process_user_input() : UserInput::PLAY;

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

        update_lives_on_screen(lives);

        process_adding_green_apple();

        update_snake_state(growing > 0);

        if (growing > 0) {
          --growing;
        }

        /* handle collision cases */
        Collision collision{check_for_collisions(g_snake_state.head)};
        /* initial_tick is a hack. We nullify collisions with the border when
         * the snake first enters the play area */
        if (initial_tick) {
          --initial_tick;
          if (collision == Collision::BORDER) {
            collision = Collision::NONE;
          }
        }
        switch (collision) {
        case Collision::APPLE:
          increment_score(
              determine_score(g_apple_locations[g_collided_apple].is_green
                                  ? ScoreSource::GREEN_APPLE
                                  : ScoreSource::RED_APPLE));
          remove_apple(g_collided_apple);
          update_lives_based_on_score(lives);
          draw_score();
          growing += APPLE_GROWTH_TICKS;
          break;
        case Collision::BORDER:
        case Collision::SNAKE:
          growing = GROWING_START + lvl_idx;
          cleanup_the_body();
          --lives;
          update_lives_on_screen(lives);
          init_snake(SNAKE_START, SNAKE_DIR);
          initial_tick = INITIAL_TICK_HACK_TUNING;
          level_is_active = false;
          /* TODO this is only for development, and should trigger a Game Over,
           * instead */
          // if (!level_is_active) {
          if (lives == 0) {
            lives = 3;
          }
          continue;
        case Collision::NONE:
          /* nothing to do */
          break;
        }

        /* draw the snake */
        update_borders(move_point(SNAKE_START, SNAKE_DIR));
        draw_snake();

        /* open the pod bay doors, HAL */
        g_level.exit_is_open = get_number_of_apples() == 0;

        /* check if the snake has exited */
        if (collision == Collision::EXIT) {
          increment_score(determine_score(ScoreSource::TIMER));
          update_lives_based_on_score(lives);
          run_cleanup_animation();
          growing = GROWING_START + lvl_idx;
          cleanup_the_body();
          level_is_active = false;
          init_snake(SNAKE_START, SNAKE_DIR);
          initial_tick = INITIAL_TICK_HACK_TUNING;
          lvl_idx = lvl_idx == snake::levels.size() - 1 ? 0 : lvl_idx + 1;
        }

        /* timer logic here? */
        process_timer();
        if (check_timer_expired()) {
          add_apples(NUMBER_OF_APPLES);
          draw_apples();
          reset_timer();
        }
        draw_timer();
      }
    }
  }

  gamepad::five::deinit();
}
} // namespace snake
