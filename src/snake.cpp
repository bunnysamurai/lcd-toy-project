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

#include "snake_tiles_constexpr.hpp"

namespace {

using grid_t = uint8_t;
using pix_t = uint16_t;

/* Some game configurations */
constexpr uint32_t TITLE_BORDER_HEIGHT{24};
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
enum struct Direction : uint8_t {
  UP = 0x0,
  RIGHT = 0x1,
  DOWN = 0x2,
  LEFT = 0x3
};

struct SnakeState {
  GridLocation head;
  Direction head_dir;
  embp::circular_array<Direction, 255U> body_vec;
};

struct LevelState {
  bool exit_is_open;
  // bool entrance_is_open;
  GridLocation exit;
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
void init_level() noexcept {
  g_level.exit_is_open = false;
  // g_level.entrance_is_open = true;
  g_level.exit = {.x = static_cast<grid_t>(g_tile_grid.grid_width >> 1),
                  .y = 0};
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

void impl_draw_along_the_body(const screen::Tile &tile) {

  auto head{g_snake_state.head};
  /* iterate through the body_vec to draw the rest */
  for (const auto dir : g_snake_state.body_vec) {
    head = move_point(head, dir);
    const auto [pixx, pixy]{to_pixel_xy(head)};
    screen::draw_tile(pixx, pixy, tile);
  }
}

void cleanup_the_body() { impl_draw_along_the_body(snake::BackgroundTile); }

void draw_snake() {
  /* for right now, we'll just draw green squares */
  impl_draw_head(snake::SnakeTile);
  impl_draw_along_the_body(snake::SnakeTile);
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
  if (lives > 0) {
    --lives;
  }
  const auto row_start{(TITLE_BORDER_HEIGHT - snake::SnakeTile.side_length) >>
                       2};
  const auto col_inc{snake::SnakeTile.side_length +
                     (snake::SnakeTile.side_length >> 1)};
  auto col_start{snake::SnakeTile.side_length >> 1};
  for (int ii = 0; ii < 5; ++ii) {
    screen::draw_tile(col_start + ii * col_inc, row_start,
                      snake::BackgroundTile);
  }
  for (; lives > 0; --lives) {
    screen::draw_tile(col_start, row_start, snake::SnakeTile);
    col_start += col_inc;
  }
}

void configure_tile_grid() noexcept {
  const screen::Dimensions display_dims{screen::get_virtual_screen_size()};

  /*
   * We use the bottom 3/4 of the available display for the play area.
   * Because sure, why not?
   */
  const auto screen_pix_y_off{TITLE_BORDER_HEIGHT};
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
  screen::set_format(screen::Format::RGB565);

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
  g_apple_locations.resize(5);
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
      g_snake_state.head.y > g_tile_grid.grid_height - 2) {
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
    init_level();
    init_snake(SNAKE_START, SNAKE_DIR);
    init_apples(); /* just places an apple somewhere */
    update_lives_on_screen(lives);
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

        Collision collision{check_for_collisions()};
        if (initial_tick) {
          initial_tick = false;
          collision = Collision::NONE;
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
