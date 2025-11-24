#include "revenge.hpp"

#include <algorithm>
#include <cstdint>
#include <tuple>
#include <utility>

#include "revenge_defs.hpp"
#include "revenge_tiles.hpp"

#include "common/BitImage.hpp"
#include "common/screen_utils.hpp"
#include "embp/circular_array.hpp"
#include "gamepad/gamepad.hpp"
#include "revenge_tiles.hpp"
#include "screen/glyphs/letters.hpp"
#include "screen/screen.hpp"
#include "screen/tile.hpp"

#include "pico/rand.h"
#include "pico/time.h"

// clang-format off
/*
    Rodent's Revenge

    Architecture and Design

    cat's have rules that govern how they move
    need "trapped" detection for the cats
      they turn into cheese, which can be eaten(or smooshed) and removed from play
    need collision detection for:
      player and moveable blocks
      player and walls
      cats and moveable blocks
      cats and walls
      player and cats
    have to keep track of the moveable blocks, and update just those that are moving
      so need to detect which blocks will move/won't move when player pushes one
    there's a score to keep track of
    there's also a timer for a level
      when it runs out, spawn more cats
    
    Cats

    Cat objects will be managed in an circulary array, similar to how the apple cart was done for snake.
      When a new one is spawn, it's added to the back of the array
      When any one is removed, we can use the erase interface to get rid of them
    The game loop will interact with the cat objects by:
      running their AI state machine to get their new position
      check for collision with the player, which results in a game over
      check for "trapped" cats, and transition them to cheese
    
    Blocks (moveable)

    Blocks will be tracked with a BitImage, like how tetris handles the playfield.
    This should make collision detection a bit quicker, as we can inspect just where the player wants to move.
    If we keep track of where the player moved from, then we can search along the row/column the player is 
    moving along to do the block-moved update.
    Block-move update is to find the segment of blocks that is being pushed, check for a collision on the 
    "far end", then set a new block at the end of the segment and remove the block at the beginning (both ops 
    on the BitImage).

    User Input
    
    We'll poll the gamepad at a separate cadence to the game loop, like we've done for tetris and snake.
    The only effects of an input is either quitting (easy) or moving.
    If a move is requested, need to process all the collision detection logic.

    Collision Logic

    Runs when a player requests a move.
    Runs when a cat moves (which is in sync with the main gameloop).

    Timer

    The timer is drawn as an actual stop watch in the original game.  Maybe I'll do the same...
    At any rate, that's an implementation detail.
    The timer operates as you would expect, so a typical "timer.time", "timer.reset" and "timer.elapsed" 
    interface should be sufficient.
    When "timer.elapsed" is true, spawn more cats.


    Subtle behaviour:
    In the original game, the mouse does not die if **it** moves onto a cat.
    However, if the cat moves onto the mouse, the mouse does die.


*/
// clang-format on

namespace revenge {
/* ===========================================================================*/
/*            ____ _____  _  _____ ___ ____  __     ___    ____               */
/*           / ___|_   _|/ \|_   _|_ _/ ___| \ \   / / \  |  _ \              */
/*           \___ \ | | / _ \ | |  | | |      \ \ / / _ \ | |_) |             */
/*            ___) || |/ ___ \| |  | | |___    \ V / ___ \|  _ <              */
/*           |____/ |_/_/   \_\_| |___\____|    \_/_/   \_\_| \_\             */
/*                                                                            */
/* ===========================================================================*/

static Timer_t g_process_stopwatch_timer{PROCESS_STOPWATCH_PERIOD_US};
static Timer_t g_stopwatch_timer{STOPWATCH_TIMEOUT_PERIOD_US};
static Timer_t g_game_timer{GAME_TICK_PERIOD_US};
static PlayGrid g_playfield;
/* our playfield keeps track of these tile types:
    000 - nothing
    001 - movable block
    010 - trap
    011 - hole
    100 - unmoveable block
    101 - cheese
*/

static Grid g_grid;
static Beast g_mouse;
static embp::circular_array<Beast, MAX_NUMBER_OF_CATS> g_cats;
static Timer_t g_stuck_timer{STUCK_IN_HOLE_PERIOD_US};
static bool g_stuck_in_hole{false};
static uint8_t g_counter;
static uint8_t g_lives{2};
static TopPanelCfg g_top_panel;
static uint32_t g_score;

void reset_global_state() noexcept {
  g_process_stopwatch_timer.period(
      PROCESS_STOPWATCH_PERIOD_US);                      /* aka 1 second */
  g_process_stopwatch_timer.reset();                     /* aka 1 second */
  g_stopwatch_timer.period(STOPWATCH_TIMEOUT_PERIOD_US); /* aka 1 second */
  g_stopwatch_timer.reset();                             /* aka 1 second */
  g_game_timer.period(GAME_TICK_PERIOD_US);
  g_game_timer.reset();
  for (auto &arr : g_playfield.m_field) {
    std::fill(std::begin(arr.m_field), std::end(arr.m_field), 0);
  }
  g_grid = Grid{};
  g_mouse = Beast{};
  g_cats.clear();
  g_stuck_timer.period(STUCK_IN_HOLE_PERIOD_US);
  g_stuck_timer.reset();
  g_stuck_in_hole = false;
  g_counter = 0;
  g_lives = 2;
  g_top_panel = TopPanelCfg{};
  g_score = 0;
}
/* ===========================================================================*/
/*                       _   _ _____ ___ _     ____                           */
/*                      | | | |_   _|_ _| |   / ___|                          */
/*                      | | | | | |  | || |   \___ \                          */
/*                      | |_| | | |  | || |___ ___) |                         */
/*                       \___/  |_| |___|_____|____/                          */
/*                                                                            */
/* ===========================================================================*/

/** @brief draw a tile on the grid
 * Primary drawing utility once the game is up and running.
 * Use lower level drawing primitives for things other than tiles-on-the-grid.
 *
 * @param xx Column location on the grid
 * @param yy Row location on the grid
 * @param tile Tile to draw.  Should probably have side_length equal to the
 * grid's scaling.
 */
void draw_grid_tile(grid_t xx, grid_t yy, const screen::Tile &tile) noexcept {
  const auto [pixx, pixy]{g_grid.to_native({.x = xx, .y = yy})};
  screen::draw_tile(pixx, pixy, tile);
}

/** @brief return a random location that is also open on the play grid
 *      probably most useful when spawning in new cats
 * @return Grid location that's guaranteed to have nothing in it (i.e. is a
 * GridObject::NOTHING )
 */
[[nodiscard]] Grid::Location find_suitable_cat_spawn() noexcept {

  /* only need 5 bits for the grid location */
  static_assert(GRID_SIZE_COLS < (1 << 5));
  static_assert(GRID_SIZE_ROWS < (1 << 5));
  static_assert(
      GRID_SIZE_ROWS == GRID_SIZE_COLS,
      "If you want a rectangle for the grid, this function needs to change");

  /*
   *
   * THE method
   * Like in snake, just reroll if the random position is unsuitable.
   *
   * A suitable place is defined as a location that is both a
   *   GridObject::NOTHING
   *   AND
   *   first 3 rows or last 3 rows or first 3 columns or last 3 columns
   *
   * a call to rand gives us 32 random bits
   * the bottom two bits map us to the section:
   *   00 - first 3 rows
   *   01 - last 3 rows
   *   10 - first 3 cols
   *   11 - last 3 cols
   *
   * the next two bits give use which row/col to use:
   *   00 - row/col 0
   *   01 - row/col 1
   *   10 - row/col 2
   *   11 - row/col 1 ( we'll bias towards the middle of the 3 possibilities )
   *
   * the next 5 bits index into the other dimension
   *
   * and reroll if the index value is out-of-range or the selected location is
   * not open
   *
   * ...
   *
   * With the strategy, I think it is technically possible for a player to
   * supress creation of a cat, which would result in an unending loop.  We
   * should probably check for this in a future update or decide not to care.
   */

  auto &&generate_other_dim_index{[](auto &randnum) {
    const auto other_dimension_index{randnum & 0b11111};
    randnum >>= 5;
    return other_dimension_index;
  }};

  auto &&generate_section_selection_offsets{[](auto &randnum) {
    const auto section_selection{randnum & 0b11};
    randnum >>= 2;
    const auto x_is_prime{static_cast<bool>(section_selection & 0b1)};
    const auto offset{static_cast<uint16_t>((GRID_SIZE_COLS - 3) *
                                            ((section_selection >> 1) & 0b1))};
    return std::make_tuple(offset, x_is_prime);
  }};

  auto &&generate_prime_dimension_index{[](auto &randnum) -> uint8_t {
    const auto result{randnum & 0b11};
    if (result == 0b11) {
      return 0b01;
    }
    return result;
  }};

  /* we use 9 bits for "roll of the dice", so we can use a single call to
   * get_rand_32 3 times before we need to call it again.*/
  static constexpr uint32_t NUMBER_OF_INNER_TRIES{3};
  for (;;) {
    auto number{get_rand_32()};

    for (uint32_t ii = 0; ii < NUMBER_OF_INNER_TRIES; ++ii) {
      const auto other_dimension_index{generate_other_dim_index(number)};
      if (other_dimension_index >= GRID_SIZE_COLS) {
        /* reroll */
        continue;
      }
      const auto [offset,
                  x_is_prime]{generate_section_selection_offsets(number)};
      const auto within_selection_index{generate_prime_dimension_index(number)};

      const Grid::Location potential{
          .x = offset * x_is_prime + within_selection_index * x_is_prime +
               other_dimension_index * (!x_is_prime),
          .y = offset * (!x_is_prime) + within_selection_index * (!x_is_prime) +
               other_dimension_index * x_is_prime};

      /* final check, make sure this location isn't on top of another cat */
      const bool no_cat_here{std::end(g_cats) ==
                             std::find_if(std::begin(g_cats), std::end(g_cats),
                                          [=](const auto &beast) {
                                            return beast.location() ==
                                                   potential;
                                          })};

      if (no_cat_here &&
          static_cast<GridObject>(g_playfield.get(potential.x, potential.y)) ==
              GridObject::NOTHING) {
        return potential;
      }
    }
  }

  return {};
}

/* ===========================================================================*/
/*              ____ ___  _     _     ___ ____ ___ ___  _   _                 */
/*             / ___/ _ \| |   | |   |_ _/ ___|_ _/ _ \| \ | |                */
/*            | |  | | | | |   | |    | |\___ \| | | | |  \| |                */
/*            | |__| |_| | |___| |___ | | ___) | | |_| | |\  |                */
/*             \____\___/|_____|_____|___|____/___\___/|_| \_|                */
/*                                                                            */
/* ===========================================================================*/

[[nodiscard]] constexpr Collision
check_for_collision(Grid::Location proposed) noexcept {
  /* first, check for the border */
  if (g_grid.out_of_bounds(proposed)) {
    return Collision::FIXED_BLOCK;
  }

  /* next, check for any collisions with kitty-kitties */
  for (const auto cat : g_cats) {
    if (proposed == cat.location()) {
      return Collision::CAT;
    }
  }

  /* finally, check the playfield */
  const auto type{
      static_cast<GridObject>(g_playfield.get(proposed.x, proposed.y))};

  return static_cast<Collision>(type);
}

/* ===========================================================================*/
/*                      ____  _____    _    ____ _____                        */
/*                     | __ )| ____|  / \  / ___|_   _|                       */
/*                     |  _ \|  _|   / _ \ \___ \ | |                         */
/*                     | |_) | |___ / ___ \ ___) || |                         */
/*                     |____/|_____/_/   \_\____/ |_|                         */
/*                                                                            */
/* ===========================================================================*/

void draw_beast(const Beast &aminal, const screen::Tile &tile) noexcept {
  draw_grid_tile(aminal.location().x, aminal.location().y, tile);
}

/** @brief Move a cat at a particular grid point
 *
 * @param point Location where the cat (likely) is.
 * @param dir Direction to move.
 *
 */
[[nodiscard]] uint32_t find_cat_at(Grid::Location point) noexcept {
  for (uint32_t idx = 0; idx < g_cats.size(); ++idx) {
    if (g_cats[idx].location() == point) {
      return idx;
    }
  }
  return g_cats.size();
}

/** @brief traverse through the moveable block row/column, given the intended
 * direction.
 *
 * @return False if this block train can't move, true if it can move.
 */
[[nodiscard]] bool
traverse_moveable_blocks(Grid::Location start, const Direction dir,
                         Grid::Location &point_of_encounter) noexcept {
  /* starting with start, keep moving in the 'dir' direction until one of the
   * following occurs:
   *   a non-moveable thing is encountered (nonmove-block, trap)
   *   a hole is encountered
   *   open space or cheese is encountered
   *   also need to check for a cat
   */
  static constexpr Grid::Location LIMITS{.x = GRID_SIZE_COLS,
                                         .y = GRID_SIZE_ROWS};
  point_of_encounter = start;
  if (point_of_encounter >= LIMITS) {
    return false;
  }
  GridObject obj{static_cast<GridObject>(
      g_playfield.get(point_of_encounter.x, point_of_encounter.y))};
  while (obj == GridObject::MOVABLE_BLOCK) {
    point_of_encounter = move(point_of_encounter, dir);
    if (point_of_encounter >= LIMITS) {
      return false;
    }
    obj = static_cast<GridObject>(
        g_playfield.get(point_of_encounter.x, point_of_encounter.y));
  }

  return obj == GridObject::CHEESE || obj == GridObject::HOLE ||
         obj == GridObject::NOTHING;
}

/** @brief do collision detection and move the mouse
 */
[[nodiscard]] Collision move_mouse(UserInput request, Beast &beast) noexcept {
  const Direction dir{static_cast<Direction>(request)};
  const Grid::Location proposed_location{beast.proposed(dir)};

  const Collision collide{check_for_collision(proposed_location)};

  Grid::Location encounter;
  /* the only thing we need to do here is answer the question, is this requested
   * move allowed? */
  switch (collide) {
  case Collision::NONE:
  case Collision::CHEESE:
  case Collision::HOLE:
    beast.location(proposed_location);
    break;
  case Collision::BLOCK:
    if (traverse_moveable_blocks(proposed_location, dir, encounter)) {

      const GridObject object_encountered{
          g_playfield.get(encounter.x, encounter.y)};

      const bool it_was_a_cat{(object_encountered == GridObject::NOTHING) &&
                              check_for_collision(encounter) == Collision::CAT};
      const bool the_cat_can_move{check_for_collision(move(encounter, dir)) ==
                                  Collision::NONE};

      if (it_was_a_cat && the_cat_can_move) {
        const auto cat_idx{find_cat_at(encounter)};
        if (cat_idx != g_cats.size()) {
          draw_beast(g_cats[cat_idx], BACKGROUND);
          g_cats[cat_idx].move(dir);
          draw_beast(g_cats[cat_idx], CAT);
        }
      }
      if (!it_was_a_cat || (it_was_a_cat && the_cat_can_move)) {
        beast.location(proposed_location);
        if (object_encountered != GridObject::HOLE) {
          g_playfield.set(static_cast<uint8_t>(GridObject::MOVABLE_BLOCK),
                          encounter.x, encounter.y);
          draw_grid_tile(encounter.x, encounter.y, BLOCK);
        } else {
          g_counter++;
        }
      }
    }
    break;
  case Collision::FIXED_BLOCK:
  case Collision::CAT:
  case Collision::TRAP:
  case Collision::MOUSE:
    break;
  }

  return collide;
}

void add_cats_to_play(uint8_t number_of_cats) noexcept {
  for (uint8_t idx = 0; idx < number_of_cats; ++idx) {
    if (!g_cats.full()) {
      g_cats.push_back(Beast{find_suitable_cat_spawn()});
      draw_beast(g_cats.back(), CAT);
    }
  }
}

[[nodiscard]] constexpr std::pair<Direction, Direction>
disposition_a_fuzzy_move(Grid::Location mouse, Grid::Location cat) noexcept {
  const int32_t xdiff{static_cast<int32_t>(cat.x) -
                      static_cast<int32_t>(mouse.x)};
  const int32_t ydiff{static_cast<int32_t>(cat.y) -
                      static_cast<int32_t>(mouse.y)};

  const bool x_is_positive{xdiff > 0};
  const bool y_is_positive{ydiff > 0};

  const bool x_is_greater{(xdiff * (-1 * !x_is_positive)) >
                          (ydiff * (-1 * !y_is_positive))};

  const auto xypolar{
      static_cast<uint8_t>((y_is_positive << 1) | x_is_positive)};
  if (x_is_greater) {
    switch (xypolar) {
    case 0b00: /* both negative */
               /* 4, 5 */
      return std::make_pair(Direction::RIGHT, Direction::DOWN_AND_RIGHT);
    case 0b01: /* x positive */
      /* 0, 7 */
      return std::make_pair(Direction::LEFT, Direction::DOWN_AND_LEFT);
    case 0b10: /* y positive */
      /* 3, 4 */
      return std::make_pair(Direction::UP_AND_RIGHT, Direction::RIGHT);
    case 0b11: /* both positive */
      /* 0, 1*/
      return std::make_pair(Direction::LEFT, Direction::UP_AND_LEFT);
    }
    return std::make_pair(Direction::RIGHT, Direction::LEFT);
  } else {
    switch (xypolar) {
    case 0b00: /* both negative */
      /* 5, 6*/
      return std::make_pair(Direction::DOWN_AND_RIGHT, Direction::DOWN);
    case 0b01: /* x positive */
      /* 6, 7 */
      return std::make_pair(Direction::DOWN, Direction::DOWN_AND_LEFT);
    case 0b10: /* y positive */
      /* 2, 3 */
      return std::make_pair(Direction::UP, Direction::UP_AND_RIGHT);
    case 0b11: /* both positive */
      /* 1, 2*/
      return std::make_pair(Direction::UP_AND_LEFT, Direction::UP);
    }
    return std::make_pair(Direction::UP, Direction::DOWN);
  }
}

[[nodiscard]] bool move_cats() {
  /* okay, so here's some real magic.

  We want to path towards the mouse, but in a simplisitic way.
  We just need to decide which 8-way adjacent grid point to move into.
  TODO If I ever add difficulty settings, one potential knob to turn is this
  pathing algo.

  Steps:
    take difference between cat's position and mouse position
    use this slope to determine which pair of candidates to move into
    pick one of the two at random

  And now, a visual

       +---+---+---+
       | 1 | 2 | 3 |
       +---+---+---+
       | 0 | C | 4 |
       +---+---+---+
       | 7 | 6 | 5 |
       +---+---+---+

  slope = [cat.x, cat.y] - [mouse.x, mouse.y]

  if x and y are positive(1), then
    if x > y
      candidates = 0, 1
    else
      candidates = 1, 2

  if x and y are negative(5), then
    if |x| > |y|
      candidates = 4, 5
    else
      candidates = 5, 6

  if only x is positive(7), then
    if x > |y|
      candidates = 0, 7
    else
      candidates = 6, 7

  if only y is positive(3), then
    if |x| > y
      candidates = 3, 4
    else
      candidates = 2, 3

  */

  static_assert(sizeof(uint32_t) * 8 > MAX_NUMBER_OF_CATS);
  uint32_t randnum{get_rand_32()};
  uint32_t sitting_cats_count{};
  for (auto &cat : g_cats) {

    bool open_found{false};
    for (const auto dir : DIRECTIONS_ARRAY) {
      /* first, check if there's a mouse adjacent */
      if (cat.proposed(dir) == g_mouse.location()) {
        draw_beast(cat, BACKGROUND);
        cat.move(dir);
        draw_beast(cat, CAT);
        return true;
      }
      /* next, see if we are surrounded by things we can't move through */
      open_found |= check_for_collision(cat.proposed(dir)) == Collision::NONE;
    }

    if (!open_found) {
      ++sitting_cats_count;
      draw_beast(cat, SITTING_CAT);
      continue;
    }

    /* finally, run pathing */
    const auto dir{[&]() {
      const auto [dir1, dir2]{
          disposition_a_fuzzy_move(g_mouse.location(), cat.location())};
      if (randnum & 0b1) {
        return dir1;
      }
      return dir2;
    }()};

    if (check_for_collision(cat.proposed(dir)) == Collision::NONE) {
      draw_beast(cat, BACKGROUND);
      cat.move(dir);
      draw_beast(cat, CAT);
    }

    randnum >>= 1;
  }

  /* if the number of sitting cats is equal to the total number of cats:
       1. turn them into cheese
       2. reinit g_cats with 2 new cats
  */
  if (sitting_cats_count == std::size(g_cats)) {
    for (auto cat : g_cats) {
      draw_beast(cat, CHEESE);
      g_playfield.set(static_cast<uint8_t>(GridObject::CHEESE),
                      cat.location().x, cat.location().y);
    }
    g_cats.clear();
    add_cats_to_play(2);
  }

  return false;
}

/* ===========================================================================*/
/*            ____  _        _ __   ______ ____  ___ ____                     */
/*           |  _ \| |      / \\ \ / / ___|  _ \|_ _|  _ \                    */
/*           | |_) | |     / _ \\ V / |  _| |_) || || | | |                   */
/*           |  __/| |___ / ___ \| || |_| |  _ < | || |_| |                   */
/*           |_|   |_____/_/   \_\_| \____|_| \_\___|____/                    */
/*                                                                            */
/* ===========================================================================*/

constexpr void load_level_onto_playgrid(PlayGrid &playfield) noexcept {
  /* for now, we'll hard-code it */
  for (uint32_t yy = 3; yy < PlayGrid::ROWS - 3; ++yy) {
    for (uint32_t xx = 3; xx < PlayGrid::COLS - 3; ++xx) {
      playfield.set(static_cast<uint8_t>(GridObject::MOVABLE_BLOCK), xx, yy);
    }
  }
  playfield.set(static_cast<uint8_t>(GridObject::HOLE), 14, 14);
  playfield.set(static_cast<uint8_t>(GridObject::UNMOVEABLE_BLOCK), 6, 6);
}

void render_playgrid(PlayGrid &playfield) {
  for (uint32_t yy = 0; yy < PlayGrid::ROWS; ++yy) {
    for (uint32_t xx = 0; xx < PlayGrid::COLS; ++xx) {
      const auto obj{static_cast<GridObject>(playfield.get(xx, yy))};
      switch (obj) {
      case GridObject::NOTHING:
        draw_grid_tile(xx, yy, BACKGROUND);
        break;
      case GridObject::MOVABLE_BLOCK:
        draw_grid_tile(xx, yy, BLOCK);
        break;
      case GridObject::TRAP:
        draw_grid_tile(xx, yy, TRAP);
        break;
      case GridObject::HOLE:
        draw_grid_tile(xx, yy, HOLE);
        break;
      case GridObject::UNMOVEABLE_BLOCK:
        draw_grid_tile(xx, yy, UNMOVEBLOCK);
        break;
      case GridObject::CHEESE:
        draw_grid_tile(xx, yy, CHEESE);
        break;
      }
    }
  }
}

/* ===========================================================================*/
/*           _   _ ____  _____ ____    ___ _   _ ____  _   _ _____ */
/*          | | | / ___|| ____|  _ \  |_ _| \ | |  _ \| | | |_   _| */
/*          | | | \___ \|  _| | |_) |  | ||  \| | |_) | | | | | | */
/*          | |_| |___) | |___|  _ <   | || |\  |  __/| |_| | | | */
/*           \___/|____/|_____|_| \_\ |___|_| \_|_|    \___/  |_| */
/*                                                                            */
/* ===========================================================================*/
[[nodiscard]] UserInput process_user_input() noexcept {
  static bool waiting_for_release{false};

  const gamepad::five::State key_pressed{gamepad::five::get()};

  if (!waiting_for_release) {
    if (key_pressed.etc) {
      waiting_for_release = true;
      return UserInput::QUIT;
    }
    if (key_pressed.up) {
      waiting_for_release = true;
      return UserInput::MOVE_UP;
    }
    if (key_pressed.down) {
      waiting_for_release = true;
      return UserInput::MOVE_DOWN;
    }
    if (key_pressed.left) {
      waiting_for_release = true;
      return UserInput::MOVE_LEFT;
    }
    if (key_pressed.right) {
      waiting_for_release = true;
      return UserInput::MOVE_RIGHT;
    }
  }
  if (!key_pressed.up && !key_pressed.down && !key_pressed.left &&
      !key_pressed.right && !key_pressed.etc) {
    waiting_for_release = false;
  }

  return UserInput::NO_ACTION;
}
/* ========================================================================= */
/*                     _   _  ___  _     _____                               */
/*                    | | | |/ _ \| |   | ____|                              */
/*                    | |_| | | | | |   |  _|                                */
/*                    |  _  | |_| | |___| |___                               */
/*                    |_| |_|\___/|_____|_____|                              */
/*                                                                           */
/* ========================================================================= */
void unstuck_the_mouse() noexcept {
  /* search for the first (or random?  or previous?) open spot to place the
   * mouse */
  const auto hole_location{g_mouse.location()};
  for (const auto dir : DIRECTIONS_ARRAY) {
    const Grid::Location proposed_location{g_mouse.proposed(dir)};
    const Collision collide{check_for_collision(proposed_location)};
    if (collide == Collision::NONE) {
      g_mouse.location(proposed_location);
      draw_beast(g_mouse, MOUSE);
      draw_grid_tile(hole_location.x, hole_location.y, HOLE);
      return;
    }
  }
}
void process_stuck_in_hole() noexcept {
  if (g_stuck_in_hole && g_stuck_timer.elapsed()) {
    g_stuck_in_hole = false;
    unstuck_the_mouse();
  }
}

void process_easter_egg() {
  static constexpr std::array LIST_OF_NAMES{"Ben",  "Mark", "Tim",
                                            "Noah", "Gabe", "Christian"};
  embp::variable_array<uint8_t, std::size(LIST_OF_NAMES)> ordered{};
  ordered.push_back(0);
  ordered.push_back(1);
  ordered.push_back(2);
  ordered.push_back(3);
  ordered.push_back(4);
  ordered.push_back(5);

  std::array<uint8_t, std::size(LIST_OF_NAMES)> rolls{};
  // don't care about performance here!  so hack this in
  for (uint32_t oidx = 0; oidx < std::size(LIST_OF_NAMES); ++oidx) {
    const auto idx{get_rand_32() % std::size(ordered)};
    rolls[oidx] = ordered[idx];
    ordered.erase(std::next(std::begin(ordered), idx));
  }

  /* now that we have our ordering, draw right over the screen */

  /* create a blank slate */
  static_assert(VIDEO_FORMAT == screen::Format::RGB565_LUT4);
  static constexpr uint32_t ROWOFF{10};
  static constexpr uint32_t COLOFF{10};
  static constexpr uint8_t BLACK4BPP{};

  const auto dims{screen::get_virtual_screen_size()};
  auto *vidbuf{screen::get_video_buffer()};
  for (uint32_t yy = ROWOFF; yy < dims.height - ROWOFF; ++yy) {
    const uint32_t rowstart{yy * dims.width};
    for (uint32_t linidx = rowstart + COLOFF;
         linidx < rowstart + (dims.width - COLOFF); linidx += 2) {
      vidbuf[linidx >> 1] = BLACK4BPP;
    }
  }

  /* draw each name in 'rolls' */
  screen::letter_4bpp_array_t buffer{};
  const screen::Tile tile_obj{.side_length = glyphs::tile::width(),
                              .format = screen::Format::RGB565_LUT4,
                              .data = std::data(buffer)};
  auto &&draw_name{
      [&](const char *str, const uint32_t row_start, const uint32_t col_start) {
        const auto ypos{row_start};
        auto xpos{col_start};

        for (size_t idx{0};; ++idx) {
          if (str[idx] == '\0') {
            break;
          }
          screen::get_letter_data_4bpp(buffer, str[idx], WHITE, BLACK);
          screen::draw_tile(xpos, ypos, tile_obj);
          xpos += glyphs::tile::width();
        }
      }};

  for (uint32_t idx = 0; idx < std::size(rolls); ++idx) {
    const auto nameidx{rolls[idx]};
    draw_name(LIST_OF_NAMES[nameidx], ROWOFF + 5 + idx * glyphs::tile::width(),
              COLOFF + 5);
  }

  /* wait for user input of any kind */
  for (;;) {
    const UserInput request{process_user_input()};
    if (request != UserInput::NO_ACTION) {
      break;
    }
  }
}

/* ========================================================================= */
/*                    _     _____     _______ ____                           */
/*                   | |   |_ _\ \   / / ____/ ___|                          */
/*                   | |    | | \ \ / /|  _| \___ \                          */
/*                   | |___ | |  \ V / | |___ ___) |                         */
/*                   |_____|___|  \_/  |_____|____/                          */
/*                                                                           */
/* ========================================================================= */
void draw_lives(uint8_t life_count) noexcept {
  std::array<uint8_t, BTLEN> mouse_with_grey_background{};

  screen::Tile mousetile{MOUSE};
  mousetile.data = std::data(mouse_with_grey_background);

  if (!screen::copy_with_replacement(MOUSE, mouse_with_grey_background, BKGRND,
                                     LGREY)) {
    printf("Unable to draw lives??\n");
  }

  /* clear the whole thing */
  screen::fillrows(
      LGREY, g_top_panel.lives_draw_start.y, g_top_panel.lives_draw_finish.y,
      g_top_panel.lives_draw_start.x, g_top_panel.lives_draw_finish.x);

  /* redraw all the lives */
  for (uint8_t ii = 0; ii < life_count; ++ii) {
    const auto xpos{ii * PIXELS_PER_GRID + g_top_panel.lives_draw_start.x};
    if (xpos > g_top_panel.lives_draw_finish.x) {
      break;
    }
    screen::draw_tile(xpos, g_top_panel.lives_draw_start.y, mousetile);
  }
}

/* ========================================================================= */
/*                     ____   ____ ___  ____  _____                          */
/*                    / ___| / ___/ _ \|  _ \| ____|                         */
/*                    \___ \| |  | | | | |_) |  _|                           */
/*                     ___) | |__| |_| |  _ <| |___                          */
/*                    |____/ \____\___/|_| \_\_____|                         */
/*                                                                           */
/* ========================================================================= */

void draw_score(uint32_t score) {
  /* convert integral score into 6 decimal digits */
  const auto digits{screen::bcd<6>(score)};

  /* convert the existing 1bpp number glyphs into double-sized 4bpp versions */
  screen::letter_4bpp_array_t letter_4bpp_data{};
  const screen::Tile letter_tile{.side_length = glyphs::tile::width(),
                                 .format = screen::Format::RGB565_LUT4,
                                 .data = std::data(letter_4bpp_data)};

  // constexpr void
  // copy_1bpptile_to_4bpp_buffer(letter_4bpp_array_t &tile_4bpp,
  //                              const screen::Tile &tile_1bpp,
  //                              const uint8_t set_word = 0b1111,
  //                              const uint8_t unset_word = 0b0000) noexcept {
  /* go from msd to lsd */

  // struct TopPanelCfg {
  //   struct Point {
  //     uint16_t x;
  //     uint16_t y;
  //   };
  //   Point lives_draw_start;
  //   Point lives_draw_finish;
  //   Point timer_center;
  //   Point score_start;
  // };
  const auto row_start{g_top_panel.score_start.y};
  auto col_start{g_top_panel.score_start.x};
  for (const auto digit : digits) {
    const auto tile_1bpp{
        glyphs::tile::decode_ascii(static_cast<char>(digit + 0x30))};
    copy_1bpptile_to_4bpp_buffer(letter_4bpp_data, tile_1bpp, BLACK, LGREY);
    screen::draw_tile(col_start, row_start, letter_tile);
    col_start += letter_tile.side_length;
  }
}

/* ========================================================================= */
/*            ____ _____ ___  ______        ___  _____ ____ _   _            */
/*           / ___|_   _/ _ \|  _ \ \      / / \|_   _/ ___| | | |           */
/*           \___ \ | || | | | |_) \ \ /\ / / _ \ | || |   | |_| |           */
/*            ___) || || |_| |  __/ \ V  V / ___ \| || |___|  _  |           */
/*           |____/ |_| \___/|_|     \_/\_/_/   \_\_| \____|_| |_|           */
/*                                                                           */
/* ========================================================================= */

/** @brief update everything about that stopwatch
 * @return True if stop watch has expired, false otherwise.
 */
bool update_stopwatch() { return true; }

/* ========================================================================= */
/*                             ____  _   _ _   _                             */
/*                            |  _ \| | | | \ | |                            */
/*                            | |_) | | | |  \| |                            */
/*                            |  _ <| |_| | |\  |                            */
/*                            |_| \_\\___/|_| \_|                            */
/*                                                                           */
/* ========================================================================= */
void draw_playgrid_border(const Grid &grid,
                          const screen::Tile border_tile) noexcept {
  /* this function should probably check for out-of-bounds conditions on the
   * physical screen....*/

  /* we are off grid... by one ;) */
  const auto grid_startpix_xy{grid.to_native({.x = 0, .y = 0})};
  const auto grid_finishpix_xy{
      grid.to_native({.x = GRID_SIZE_COLS - 1, .y = GRID_SIZE_ROWS - 1})};

  const uint32_t start_row{grid_startpix_xy.y - border_tile.side_length};
  const uint32_t start_col{grid_startpix_xy.x - border_tile.side_length};

  const uint32_t finish_row{grid_finishpix_xy.y + border_tile.side_length};
  const uint32_t finish_col{grid_finishpix_xy.x + border_tile.side_length};

  /* draw top border */
  for (uint32_t col = start_col; col <= finish_col;
       col += border_tile.side_length) {
    screen::draw_tile(col, start_row, border_tile);
  }
  /* draw bottom border */
  for (uint32_t col = start_col; col <= finish_col;
       col += border_tile.side_length) {
    screen::draw_tile(col, finish_row, border_tile);
  }
  /* draw left border */
  for (uint32_t row = start_row + border_tile.side_length;
       row <= finish_row - border_tile.side_length;
       row += border_tile.side_length) {
    screen::draw_tile(start_col, row, border_tile);
  }
  /* draw right border */
  for (uint32_t row = start_row + border_tile.side_length;
       row <= finish_row - border_tile.side_length;
       row += border_tile.side_length) {
    screen::draw_tile(finish_col, row, border_tile);
  }
}

void screen_init() noexcept {
  /* we are a color application */
  screen::set_format(revenge::VIDEO_FORMAT);
  screen::init_clut(revenge::Palette.data(), revenge::Palette.size());

  /* we use a black background */
  screen::fill_screen((LGREY << 4) | LGREY);
}

void game_init() noexcept {
  const auto dims{screen::get_virtual_screen_size()};

  /* g_grid is the playing grid, which constrains how and where the mouse can
   * move */
  g_grid = Grid{Grid::GridCfg{
      .xdimension = {.off =
                         (dims.width - GRID_SIZE_COLS * PIXELS_PER_GRID) >> 1,
                     .scale = PIXELS_PER_GRID},
      .ydimension = {.off =
                         (dims.height - GRID_SIZE_ROWS * PIXELS_PER_GRID) >> 1,
                     .scale = PIXELS_PER_GRID},
      .grid_width = GRID_SIZE_COLS,
      .grid_height = GRID_SIZE_ROWS}};

  /* g_top_panel will have the timer, lives, and score.  It is defined as all
     the area above the grid.

        +----------------------------------------------------------+
        |                                                          |
        |  m m m                    |                  567         |
        |                            \                             |
        +----------------------------------------------------------+

  */
  const auto top_panel_width{dims.width};
  const auto top_panel_height{g_grid.config().ydimension.off};
  g_top_panel = TopPanelCfg{
      .lives_draw_start = {.x = static_cast<uint16_t>(PIXELS_PER_GRID >> 1),
                           .y = static_cast<uint16_t>((top_panel_height >> 1) -
                                                      PIXELS_PER_GRID)},
      .lives_draw_finish = {.x = static_cast<uint16_t>((PIXELS_PER_GRID >> 1) +
                                                       PIXELS_PER_GRID * 6),
                            .y = static_cast<uint16_t>(top_panel_height >> 1)},
      .timer_center = {.x = static_cast<uint16_t>(top_panel_width >> 1),
                       .y = static_cast<uint16_t>(top_panel_height >> 1)},
      .score_start = {.x = static_cast<uint16_t>(
                          top_panel_width - (6 * glyphs::tile::width() +
                                             (glyphs::tile::width() >> 1))),
                      .y = static_cast<uint16_t>((top_panel_height >> 1) -
                                                 PIXELS_PER_GRID)}};

  draw_lives(g_lives);

  load_level_onto_playgrid(g_playfield);

  render_playgrid(g_playfield);

  /* draw the border, which lies outside the grid
   * This is the only time it is done for the life of the program
   */
  draw_playgrid_border(g_grid, UNMOVEBLOCK);
}

void restore_entire_screen_state() {
  screen::fill_screen((LGREY << 4) | LGREY);
  draw_lives(g_lives);
  render_playgrid(g_playfield);
  draw_playgrid_border(g_grid, UNMOVEBLOCK);
  draw_beast(g_mouse, g_stuck_in_hole ? MOUSE_IN_HOLE : MOUSE);
  for (auto &cat : g_cats) {
    draw_beast(cat, CAT);
  }
  draw_score(g_score);
}

void respawn_mouse() {
  /* initalize where the mouse goes */
  g_mouse.location({.x = GRID_SIZE_COLS >> 1, .y = GRID_SIZE_ROWS >> 1});
  g_playfield.set(static_cast<uint8_t>(GridObject::NOTHING),
                  GRID_SIZE_COLS >> 1, GRID_SIZE_ROWS >> 1);
  draw_beast(g_mouse, MOUSE);
}

void run() {
  reset_global_state();
  screen_init();
  game_init();
  gamepad::five::init();

  /* initalize where the mouse goes */
  respawn_mouse();

  /* initialize the cats */
  add_cats_to_play(2);

  draw_score(g_score);

  g_stopwatch_timer.reset();
  g_game_timer.reset();
  for (;;) {

    /* user input loop, which runs on a release of a button */
    const UserInput request{process_user_input()};
    if (request == UserInput::QUIT) {
      break;
    }
    if (g_counter == 6) {
      process_easter_egg();
      g_counter = 0;
      restore_entire_screen_state();
    }
    if (request != UserInput::NO_ACTION && !g_stuck_in_hole) {
      const auto location_before_move{g_mouse.location()};
      const auto collision{move_mouse(request, g_mouse)};

      /* TODO we need to take action on the various collision types here... */

      /* Here's the action to take when the mouse didn't get Eaten, Trapped,
       * Fell In A Hole, or Ran Over By Yarn */
      if (location_before_move != g_mouse.location()) {
        /* the mouse has moved.  now we need to decide a few things:
              did the mouse die?
              if not, then decide
                how to draw the mouse
                process other side effects (namely, stuck-in-a-hole)
           */
        bool mouse_died = false;

        switch (collision) {
        case Collision::TRAP:
          mouse_died = true;
          break;
        case Collision::HOLE:
          g_stuck_in_hole = true;
          g_stuck_timer.reset();
          break;
        case Collision::CHEESE:
          g_score += 100;
          draw_score(g_score);
          break;
        case Collision::NONE:
        case Collision::BLOCK:
        case Collision::FIXED_BLOCK:
        case Collision::CAT:
        case Collision::MOUSE:
          break;
        }

        if (mouse_died) {
          break;
        }
        g_playfield.set(static_cast<uint8_t>(GridObject::NOTHING),
                        location_before_move.x, location_before_move.y);
        draw_beast(g_mouse, g_stuck_in_hole ? MOUSE_IN_HOLE : MOUSE);
        draw_grid_tile(location_before_move.x, location_before_move.y,
                       BACKGROUND);
      }
    }

    /* game loop, which processes the cats (that sounds inhumane) */
    if (g_game_timer.elapsed()) {
      g_game_timer.reset();
      if (move_cats()) {
        /* cat ate the mouse!  GAME OVER */
        if (g_lives == 0) {
          screen::melt(BLACK);
          break;
        }
        --g_lives;
        draw_lives(g_lives);
        respawn_mouse();
      }
      process_stuck_in_hole();
    }
    // if (g_stopwatch_timer.elapsed()) {
    //   g_stopwatch_timer.reset();
    //   if (update_stopwatch()) {
    //     add_cats_to_play(2);
    //   }
    // }

    sleep_us(1000);
  }
}

} // namespace revenge