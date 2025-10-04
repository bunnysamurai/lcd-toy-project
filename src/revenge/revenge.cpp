#include "revenge.hpp"

#include <cstdint>
#include <regex>

#include "revenge_defs.hpp"
#include "revenge_tiles.hpp"

#include "common/BitImage.hpp"
#include "embp/circular_array.hpp"
#include "gamepad/gamepad.hpp"
#include "revenge_tiles.hpp"
#include "screen/screen.hpp"

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
static Timer_t g_game_timer{GAME_TICK_PERIOD_US};
static Timer_t g_input_timer{INPUT_POLL_PERIOD_US};
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

/* ===========================================================================*/
/*                       _   _ _____ ___ _     ____                           */
/*                      | | | |_   _|_ _| |   / ___|                          */
/*                      | | | | | |  | || |   \___ \                          */
/*                      | |_| | | |  | || |___ ___) |                         */
/*                       \___/  |_| |___|_____|____/                          */
/*                                                                            */
/* ===========================================================================*/
void draw_grid_tile(grid_t xx, grid_t yy, const screen::Tile &tile) noexcept {
  const auto [pixx, pixy]{g_grid.to_native({.x = xx, .y = yy})};
  screen::draw_tile(pixx, pixy, tile);
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
  if (proposed.x > GRID_SIZE_COLS || proposed.y > GRID_SIZE_ROWS) {
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

[[nodiscard]] Collision move_beast(UserInput request, Beast &beast) noexcept {
  const Direction dir{static_cast<Direction>(request)};
  const Grid::Location proposed_location{beast.proposed(dir)};

  const Collision collide{check_for_collision(proposed_location)};

  if (collide != Collision::FIXED_BLOCK) {
    beast.location(proposed_location);
  }

  return collide;
}

/* ===========================================================================*/
/*               ____  _        _ __   ______ ____  ___ ____                  */
/*              |  _ \| |      / \\ \ / / ___|  _ \|_ _|  _ \                 */
/*              | |_) | |     / _ \\ V / |  _| |_) || || | | |                */
/*              |  __/| |___ / ___ \| || |_| |  _ < | || |_| |                */
/*              |_|   |_____/_/   \_\_| \____|_| \_\___|____/                 */
/*                                                                            */
/* ===========================================================================*/

constexpr void load_level_onto_playgrid(PlayGrid &playfield) noexcept {
  /* for now, we'll hard-code it */
  for (uint32_t yy = 3; yy < PlayGrid::ROWS - 3; ++yy) {
    for (uint32_t xx = 3; xx < PlayGrid::COLS - 3; ++xx) {
      playfield.set(static_cast<uint8_t>(GridObject::MOVABLE_BLOCK), xx, yy);
    }
  }
  playfield.set(static_cast<uint8_t>(GridObject::UNMOVEABLE_BLOCK), 14, 14);
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
/*           _   _ ____  _____ ____    ___ _   _ ____  _   _ _____            */
/*          | | | / ___|| ____|  _ \  |_ _| \ | |  _ \| | | |_   _|           */
/*          | | | \___ \|  _| | |_) |  | ||  \| | |_) | | | | | |             */
/*          | |_| |___) | |___|  _ <   | || |\  |  __/| |_| | | |             */
/*           \___/|____/|_____|_| \_\ |___|_| \_|_|    \___/  |_|             */
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

/* ===========================================================================*/
/*                             ____  _   _ _   _                              */
/*                            |  _ \| | | | \ | |                             */
/*                            | |_) | | | |  \| |                             */
/*                            |  _ <| |_| | |\  |                             */
/*                            |_| \_\\___/|_| \_|                             */
/*                                                                            */
/* ===========================================================================*/
void screen_init() noexcept {
  /* we are a color application */
  screen::set_format(revenge::VIDEO_FORMAT);
  screen::init_clut(revenge::Palette.data(), revenge::Palette.size());

  /* we use a black background */
  screen::clear_screen();
}
void game_init() noexcept {
  const auto dims{screen::get_virtual_screen_size()};

  g_grid = Grid{Grid::GridCfg{
      .xdimension = {.off =
                         (dims.width - GRID_SIZE_COLS * PIXELS_PER_GRID) >> 1,
                     .scale = PIXELS_PER_GRID},
      .ydimension = {.off =
                         (dims.height - GRID_SIZE_ROWS * PIXELS_PER_GRID) >> 1,
                     .scale = PIXELS_PER_GRID},
      .grid_width = GRID_SIZE_COLS,
      .grid_height = GRID_SIZE_ROWS}};

  load_level_onto_playgrid(g_playfield);

  render_playgrid(g_playfield);
}

void run() {
  screen_init();
  game_init();
  gamepad::five::init();

  /* initalize where the mouse goes */
  g_mouse.location({.x = GRID_SIZE_COLS >> 1, .y = GRID_SIZE_ROWS >> 1});
  draw_beast(g_mouse, MOUSE);

  for (;;) {

    const UserInput request{process_user_input()};
    if (request == UserInput::QUIT) {
      break;
    }

    const auto current_location{g_mouse.location()};
    const auto collision{move_beast(request, g_mouse)};
    if (current_location != g_mouse.location() &&
        collision != Collision::FIXED_BLOCK) {
      draw_grid_tile(current_location.x, current_location.y, BACKGROUND);
    }
    if (current_location != g_mouse.location()) {
      draw_beast(g_mouse, MOUSE);
    }

    sleep_us(1000);
  }
}

} // namespace revenge