#include "tetris.hpp"
#include "tetris_defs.hpp"
#include "tetris_tiles_constexpr.hpp"

#include "pico/rand.h"
#include "pico/time.h"

#include "common/BitImage.hpp"
#include "common/screen_utils.hpp"
#include "gamepad/gamepad.hpp"

// #define PRINT_DEBUG_MSG

#ifdef PRINT_DEBUG_MSG
#include "pico/printf.h"
#endif

/* Tetris for Windows is 10 x 20 grid points
 *
 * our origin is in the top left corner, and rows increase as you move down
 *
 *  column
 *   0123456789
 *  +----------+
 *  |          |  row 0
 *  |          |  row 1
 *  |          |  .
 *  |          |  .
 *  |          |  .
 *  |          |
 *  |          |
 *  |          |
 *  |          |
 *  |          |
 *  |          |
 *  |          |
 *  |          |
 *  |          |
 *  |          |
 *  |          |
 *  |          |
 *  |          |
 *  |          |
 *  |          | row 19
 *  +----------+
 *
 */

/* Tetriminos
 *
 * As we all know, there are seven:
 *
 *    ++++  A
 *
 *    +++   +++  B/C
 *      +   +
 *
 *    ++    ++   D/E
 *     ++  ++
 *
 *     +    ++   F and G
 *    +++   ++
 *
 *
 * I'm going to steal, slightly, from Rheolism, and use a flower pattern for
 * handling rotation
 *
 *         01
 *        AB23
 *        9854
 *         76
 *
 *   A: If at 1256, 9 incr -> AB23 -> 78B0 -> 4589 -> 1256
 *   B: If at B234, 9 incr -> 8B01 -> etc
 *   and so on.
 *
 * However, Tetris for Windows has different behaviour:
 *
 * B/C are in a 3x3 box
 *                              +
 *  Rotates around this one --> *
 *                              + +
 *
 *  So flower pattern is :     012
 *                             7*3
 *                             654
 *  and increment by 2 for each rotation
 *
 *
 */

namespace tetris {

/* ========================================================================== */
/*          ____ _____  _  _____ ___ ____  __     ___    ____                 */
/*         / ___|_   _|/ \|_   _|_ _/ ___| \ \   / / \  |  _ \                */
/*         \___ \ | | / _ \ | |  | | |      \ \ / / _ \ | |_) |               */
/*          ___) || |/ ___ \| |  | | |___    \ V / ___ \|  _ <                */
/*         |____/ |_/_/   \_\_| |___\____|    \_/_/   \_\_| \_\               */
/*                                                                            */
/* ========================================================================== */
/* GUI related, there are really configurations that could live elsewhere */
static constexpr uint32_t GUI_OUTSIDE_BORDER_WIDTH{2};
static constexpr uint32_t GUI_PREVIEW_BORDER{4};
static constexpr auto GUI_UNDERLAY_COLOR_MAIN{LGREY};
static constexpr auto GUI_UNDERLAY_COLOR_SCNDRY{WHITE};
static constexpr auto GUI_TEXT_COLOR{BLACK};

/* Gameplay related */
static ThreeBitImage<PLAY_NO_COLS, PLAY_NO_ROWS> g_playfield;
static bool g_pending_commit{false};
static bool g_is_active{false};
static Tetrimino g_next_tetrimino{Tetrimino::Code::A};
static Tetrimino g_tetrimino{Tetrimino::Code::A};
static Tetrimino g_prvtetrimino{Tetrimino::Code::A};
static Location g_location;
static Location g_prvlocation; /* used for clearing? */
static uint32_t g_level{0};
static TileGridCfg g_play_cfg;
static GuiCfg g_gui;
static uint32_t g_lines_scored{};
static absolute_time_t g_game_tick_us{INITIAL_GAME_TICK_US};
static uint32_t g_points_scored{};
static constexpr std::array<uint32_t, 9> g_level_thresholds{10, 20, 30, 40, 50,
                                                            60, 70, 80, 90};
static constexpr std::array<uint8_t, 10> g_level_color{
    MAGENTA, RED, LBLUE, LMAGENTA, BLUE, LYELLOW, CYAN, YELLOW, DRKGRY, LGREY};

/* ========================================================================== */
/*                      _______        ______                                 */
/*                     |  ___\ \      / /  _ \                                */
/*                     | |_   \ \ /\ / /| | | |                               */
/*                     |  _|   \ V  V / | |_| |                               */
/*                     |_|      \_/\_/  |____/                                */
/*                                                                            */
/* ========================================================================== */
void change_background_color(const uint8_t palette_index) noexcept;

/* ========================================================================== */
/*                    _   _ _____ ___ _     ____                              */
/*                   | | | |_   _|_ _| |   / ___|                             */
/*                   | | | | | |  | || |   \___ \                             */
/*                   | |_| | | |  | || |___ ___) |                            */
/*                    \___/  |_| |___|_____|____/                             */
/*                                                                            */
/* ========================================================================== */

void reset_all_global_state() {
  g_playfield = ThreeBitImage<PLAY_NO_COLS, PLAY_NO_ROWS>{};
  g_pending_commit = false;
  g_is_active = false;
  g_tetrimino = Tetrimino{Tetrimino::Code::A};
  g_prvtetrimino = Tetrimino{Tetrimino::Code::A};
  g_location = Location{};
  g_prvlocation = Location{};
  g_level = 0;
  g_play_cfg = TileGridCfg{};
  g_lines_scored = 0;
  g_gui = GuiCfg{};
  g_points_scored = 0;
}

struct TimeClock {
  absolute_time_t last_touch_point;
};

constexpr void increment(TimeClock &clk, absolute_time_t intrv) noexcept {
  clk.last_touch_point += intrv;
}
[[nodiscard]] int64_t get_elapsed(const TimeClock &clk) noexcept {
  return absolute_time_diff_us(clk.last_touch_point, get_absolute_time());
}

[[nodiscard]] ScreenLocation to_pixel_xy(Location grid_xy) noexcept {
  /* TODO consider adding range checks here?  Or should it go somewhere else? */
  return {.x = static_cast<uint32_t>(grid_xy.x * g_play_cfg.xdimension.scale +
                                     g_play_cfg.xdimension.off),
          .y = static_cast<uint32_t>(grid_xy.y * g_play_cfg.ydimension.scale +
                                     g_play_cfg.ydimension.off)};
}

#ifdef PRINT_DEBUG_MSG
void print_playfield() {
  static constexpr auto TOP_HEADER{
      "    | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 |\n"
      "    |---------------------------------------|"};
  printf("%s\n", TOP_HEADER);
  for (size_t yy = 0; yy < PLAY_NO_ROWS; ++yy) {
    printf(" %02d ", yy);
    for (size_t xx = 0; xx < PLAY_NO_COLS; ++xx) {
      const auto idx{g_playfield.get(xx, yy)};
      printf("| %01d ", idx);
    }
    printf("|\n");
  }
  printf("\n");
}
#endif

/** @brief draw a rectangle
 *
 * @param topy
 * @param leftx
 * @param boty
 * @param rightx
 * @param coloridx Index into the color palette
 * @param thickness '0' is special, it means "filled, please"
 *
 */
void scoring_gui_draw_rectangle_primitive(uint32_t topy, uint32_t leftx,
                                          uint32_t boty, uint32_t rightx,
                                          uint8_t coloridx,
                                          uint32_t thickness) {

  /* our stack allocated tile */
  const screen::Tile tile{
      .side_length = 1, .format = VIDEO_FORMAT, .data = &coloridx};

  if (thickness == 0) {
    for (uint32_t yy = topy; yy < boty; ++yy) {
      for (uint32_t xx = leftx; xx < rightx; ++xx) {
        screen::draw_tile(xx, yy, tile);
      }
    }
  } else {
    /* bot row */
    for (uint32_t yy = boty - thickness; yy < boty; ++yy) {
      for (uint32_t xx = leftx; xx < rightx; ++xx) {
        screen::draw_tile(xx, yy, tile);
      }
    }
    /* right column */
    for (uint32_t yy = topy; yy < boty; ++yy) {
      for (uint32_t xx = rightx - thickness; xx < rightx; ++xx) {
        screen::draw_tile(xx, yy, tile);
      }
    }
    /* top row */
    for (uint32_t yy = topy; yy < topy + thickness; ++yy) {
      for (uint32_t xx = leftx; xx < rightx; ++xx) {
        screen::draw_tile(xx, yy, tile);
      }
    }
    /* left column */
    for (uint32_t yy = topy; yy < boty; ++yy) {
      for (uint32_t xx = leftx; xx < leftx + thickness; ++xx) {
        screen::draw_tile(xx, yy, tile);
      }
    }
  }
}
/* ========================================================================== */
/*          _   _ ____  _____ ____    ___ _   _ ____  _   _ _____             */
/*         | | | / ___|| ____|  _ \  |_ _| \ | |  _ \| | | |_   _|            */
/*         | | | \___ \|  _| | |_) |  | ||  \| | |_) | | | | | |              */
/*         | |_| |___) | |___|  _ <   | || |\  |  __/| |_| | | |              */
/*          \___/|____/|_____|_| \_\ |___|_| \_|_|    \___/  |_|              */
/*                                                                            */
/* ========================================================================== */
enum struct UserInput { LEFT, RIGHT, UP, DOWN, QUIT, NONE, DROP };

[[nodiscard]] UserInput process_user_input() noexcept {
  static bool waiting_for_release{false};

  const gamepad::five::State state{gamepad::five::get()};

  if (!waiting_for_release) {
    if (state.up) {
      waiting_for_release = true;
      return UserInput::UP;
    }
    if (state.down) {
      waiting_for_release = true;
      return UserInput::DOWN;
    }
    if (state.left) {
      waiting_for_release = true;
      return UserInput::LEFT;
    }
    if (state.right) {
      waiting_for_release = true;
      return UserInput::RIGHT;
    }
    if (state.etc) {
      return UserInput::QUIT;
    }
  }

  if (!state.up && !state.down && !state.left && !state.right && !state.etc) {
    waiting_for_release = false;
  }

  return UserInput::NONE;
}

/* ========================================================================== */
/*          ____  _        _ __   _______ ___ _____ _     ____                */
/*         |  _ \| |      / \\ \ / /  ___|_ _| ____| |   |  _ \               */
/*         | |_) | |     / _ \\ V /| |_   | ||  _| | |   | | | |              */
/*         |  __/| |___ / ___ \| | |  _|  | || |___| |___| |_| |              */
/*         |_|   |_____/_/   \_\_| |_|   |___|_____|_____|____/               */
/*                                                                            */
/* ========================================================================== */
auto get_position(int pos) -> Location {
  switch (pos) {
  case 0x0:
    return {.x = 1, .y = 0};
  case 0x1:
    return {.x = 2, .y = 0};
  case 0x2:
    return {.x = 2, .y = 1};
  case 0x3:
    return {.x = 3, .y = 1};
  case 0x4:
    return {.x = 3, .y = 2};
  case 0x5:
    return {.x = 2, .y = 2};
  case 0x6:
    return {.x = 2, .y = 3};
  case 0x7:
    return {.x = 1, .y = 3};
  case 0x8:
    return {.x = 1, .y = 2};
  case 0x9:
    return {.x = 0, .y = 2};
  case 0xA:
    return {.x = 0, .y = 1};
  case 0xB:
    return {.x = 1, .y = 1};
  }
  return {};
}

void draw_tetrimino_tile_pix(uint32_t tile_idx, uint32_t pixx,
                             uint32_t pixy) noexcept {
  screen::draw_tile(pixx, pixy, TETRIMINO_TILES[tile_idx]);
}
void draw_tetrimino_impl_pix(const Tetrimino tetrimino, const Location curloc,
                             const uint8_t tile_idx) noexcept {
  /* we'll hand roll this */

  /* tetrimino lives in a 4x4 grid
   * rows 0 and 3 are just columns 1 and 2
   * rows 1 and 2 are all 4 columns
   * of course we've mapped each bit to a certain location in this 4x4 grid
   */
  const auto bits{tetrimino.data()};
  for (int bit_position = 0; bit_position < 0xC; ++bit_position) {
    const auto [x, y]{get_position(bit_position)};
    if ((bits >> bit_position) & 0b1) {
      draw_tetrimino_tile_pix(tile_idx, curloc.x + x * SQUARE_SIZE,
                              curloc.y + y * SQUARE_SIZE);
    }
  }
}
void draw_tetrimino_tile(uint32_t tile_idx, uint32_t grid_x,
                         uint32_t grid_y) noexcept {
  const auto [xx, yy]{to_pixel_xy({.x = grid_x, .y = grid_y})};
  draw_tetrimino_tile_pix(tile_idx, xx, yy);
}
void draw_tetrimino_impl(const Tetrimino tetrimino, const Location curloc,
                         const uint8_t tile_idx) noexcept {
  const auto [xx, yy]{to_pixel_xy({.x = curloc.x, .y = curloc.y})};
  draw_tetrimino_impl_pix(tetrimino, {.x = xx, .y = yy}, tile_idx);
}

void draw_tetrimino() noexcept {
  static Tetrimino previous_spawned{g_next_tetrimino};
  draw_tetrimino_impl(g_prvtetrimino, g_prvlocation,
                      to_index(Tetrimino::Code::BLANK));
  draw_tetrimino_impl(g_tetrimino, g_location, to_index(g_tetrimino.code()));
  g_prvlocation = g_location;

  /* here we'll also draw the preview */
  const Location loc{.x = g_gui.piece_preview.x + GUI_PREVIEW_BORDER,
                     .y = g_gui.piece_preview.y + GUI_PREVIEW_BORDER};
  draw_tetrimino_impl_pix(previous_spawned, loc,
                          std::size(TETRIMINO_TILES) - 1);
  draw_tetrimino_impl_pix(g_next_tetrimino, loc,
                          to_index(g_next_tetrimino.code()));
  previous_spawned = g_next_tetrimino;
}
void draw_playfield() noexcept {
  for (uint32_t yy = 0; yy < PLAY_NO_ROWS; ++yy) {
    for (uint32_t xx = 0; xx < PLAY_NO_COLS; ++xx) {
      const auto tile_index{g_playfield.get(xx, yy)};
      draw_tetrimino_tile(tile_index, xx, yy);
    }
  }
}
void draw_playfield_border() {
  static constexpr uint32_t playfield_inset{4};

  const auto topy{g_play_cfg.ydimension.off - playfield_inset};
  const auto leftx{g_play_cfg.xdimension.off - playfield_inset};
  const auto rightx{leftx +
                    g_play_cfg.grid_width * g_play_cfg.xdimension.scale +
                    2 * playfield_inset};
  const auto boty{topy + g_play_cfg.grid_height * g_play_cfg.ydimension.scale +
                  2 * playfield_inset};

  /* bot */
  scoring_gui_draw_rectangle_primitive(boty - playfield_inset, leftx, boty,
                                       rightx, LGREY, 0);
  /* right */
  scoring_gui_draw_rectangle_primitive(topy, rightx - playfield_inset, boty,
                                       rightx, LGREY, 0);
  /* top */
  scoring_gui_draw_rectangle_primitive(topy, leftx, topy + playfield_inset,
                                       rightx, LGREY, 0);
  /* left */
  scoring_gui_draw_rectangle_primitive(topy, leftx, boty,
                                       leftx + playfield_inset, LGREY, 0);

  scoring_gui_draw_rectangle_primitive(topy, rightx - (playfield_inset >> 1),
                                       boty, rightx, DRKGRY, 0);
  scoring_gui_draw_rectangle_primitive(boty - (playfield_inset >> 1), leftx,
                                       boty, rightx, DRKGRY, 0);
  scoring_gui_draw_rectangle_primitive(
      topy, leftx, topy + (playfield_inset >> 1), rightx, WHITE, 0);
  scoring_gui_draw_rectangle_primitive(
      topy, leftx, boty, leftx + (playfield_inset >> 1), WHITE, 0);
}

/* ========================================================================== */
/*                     _     ___ _   _ _____ ____                             */
/*                    | |   |_ _| \ | | ____/ ___|                            */
/*                    | |    | ||  \| |  _| \___ \                            */
/*                    | |___ | || |\  | |___ ___) |                           */
/*                    |_____|___|_| \_|_____|____/                            */
/*                                                                            */
/* ========================================================================== */
/** @brief finds which rows have all columns set
 */
[[nodiscard]] uint32_t get_full_set_rows() noexcept {
  uint32_t rows_to_clear{0};
  for (uint32_t yy = 0; yy < PLAY_NO_ROWS; ++yy) {
    bool full{true};
    for (uint32_t xx = 0; xx < PLAY_NO_COLS; ++xx) {
      full &= (to_index(Tetrimino::Code::BLANK) != g_playfield.get(xx, yy));
    }
    rows_to_clear |= (1 << yy) * full;
  }

  return rows_to_clear;
}

void move_this_row_segment(uint16_t start, uint16_t stop) {
  /*
    if rows 15 and 16 are set, then our start is 15 and stop is 17

    we need to remove two rows, so the very top row will move down 2 rows

    in order not to lose information, we start overwritting the rows that are
    going away, anyways.  so we copy row (start-1) to row (stop-1), and continue
    until we reach row 1

    rowidx = start - 1;
    dist = stop - start;
    while(rowidx>0)
    {
      copy_row(rowidx, rowidx+dist)
      --rowidx;
    }
    here, we always blank row 0
  */

  auto &&copy_row{[](int32_t from, int32_t to) {
    for (int32_t xx = 0; xx < PLAY_NO_COLS; ++xx) {
      g_playfield.set(g_playfield.get(xx, from), xx, to);
    }
  }};

  const auto dist{stop - start};
  auto rowidx{int32_t{start} - 1};

  while (rowidx > 0) {
    copy_row(rowidx, rowidx + dist);
    --rowidx;
  }
  for (uint32_t xx = 0; xx < PLAY_NO_COLS; ++xx) {
    g_playfield.set(to_index(Tetrimino::Code::BLANK), xx, 0);
  }
}

/** @brief update the frozen blocks grid
 *
 * @return Number of lines removed.  Could be zero.
 */
[[nodiscard]] uint32_t clear_out_complete_lines_in_playfield() noexcept {
  /* iterate through the bit image */
  /* complete lines are NOT always continguous in the y-direction, so we should
   * keep that in mind.
   *
   * we are looking for rows where all the columns are set
   */
  uint32_t lines_cleared{};

  auto set_rows_mask{get_full_set_rows()};
  if (set_rows_mask == 0) {
    return lines_cleared;
  }

  /* we'll run a cute little state machine here
   *
   * start starts off as "LOOKING"
   *
   * we then start popping bits off of set_rows_mask from the msb
   *
   * when we hit a 1, we record which bit this is, then set state to
   * "NOTLOOKING"
   *
   * when we hit a 0, we record which bit this is, then call
   * move_this_row_segment afterwards, we set state back to LOOKING
   */

  static constexpr bool LOOKING{true};
  static constexpr bool NOTLOOKING{false};
  bool state{LOOKING};

  uint16_t starty;
  uint16_t stopy;
  for (uint32_t idx = 0; idx < PLAY_NO_ROWS; ++idx) {
    const bool bitfound{1 == ((set_rows_mask >> idx) & 0b1)};

    if (bitfound && (state == LOOKING)) {
      starty = idx;
      state = NOTLOOKING;
    }
    if (!bitfound && (state == NOTLOOKING)) {
      stopy = idx;
      state = LOOKING;
      move_this_row_segment(starty, stopy);
      lines_cleared += stopy - starty;
    }
  }
  /* if we got through the rest of the rows without finding another clear bit,
   * then we've reached the bottom of the playfield */
  if (state == NOTLOOKING) {
    move_this_row_segment(starty, PLAY_NO_ROWS);
    lines_cleared += PLAY_NO_ROWS - starty;
  }
  return lines_cleared;
}
[[nodiscard]] uint32_t check_for_complete_lines() noexcept {
  const auto lines{clear_out_complete_lines_in_playfield()};
  if (lines > 0) {
    draw_playfield();
  }
  return lines;
}

/* ========================================================================== */
/*                           ____ _   _ ___                                   */
/*                          / ___| | | |_ _|                                  */
/*                         | |  _| | | || |                                   */
/*                         | |_| | |_| || |                                   */
/*                          \____|\___/|___|                                  */
/*                                                                            */
/* ========================================================================== */

void scoring_gui_draw_underlay() {
  /* some helpers */
  const uint32_t topy{g_gui.scoring_box_start.y};
  const uint32_t leftx{g_gui.scoring_box_start.x};
  const uint32_t rightx{leftx + g_gui.scoring_box_width};
  const uint32_t boty{topy + g_gui.scoring_box_height};

  /* our stack allocated tile */
  uint8_t coloridx;
  const screen::Tile tile{
      .side_length = 1, .format = VIDEO_FORMAT, .data = &coloridx};

  /* first, draw a box */
  scoring_gui_draw_rectangle_primitive(topy, leftx, boty, rightx, LGREY, 0);

  /* then, add detail */
  /* bot row and right column */
  scoring_gui_draw_rectangle_primitive(topy, leftx, boty, rightx, DRKGRY,
                                       GUI_OUTSIDE_BORDER_WIDTH);
  /* top row */
  scoring_gui_draw_rectangle_primitive(
      topy, leftx, topy + GUI_OUTSIDE_BORDER_WIDTH, rightx, WHITE, 0);
  /* left column */
  scoring_gui_draw_rectangle_primitive(
      topy, leftx, boty, leftx + GUI_OUTSIDE_BORDER_WIDTH, WHITE, 0);

  /* presto! Darth Vader */
}

/** @brief write text anywhere on the screen
 *
 *  Originally intented to write text within the scoring gui.
 *
 * @param str The string to write
 * @param ypos the offset, in absolute pixel space
 */
void scoring_gui_write_text(const char *str, uint32_t yoffset) {
  /* all specified in pixel space */

  /* temp buffer for bit-expanded characters */
  screen::letter_4bpp_array_t buffer;
  const screen::Tile tile{.side_length = glyphs::tile::width(),
                          .format = VIDEO_FORMAT,
                          .data = std::data(buffer)};

  /* "line score" underlay and text */
  const auto ypos{yoffset};
  auto xpos{g_gui.line_score_text.x};

  for (size_t idx{0};; ++idx) {
    if (str[idx] == '\0') {
      break;
    }
    screen::get_letter_data_4bpp(buffer, str[idx], GUI_TEXT_COLOR,
                                 GUI_UNDERLAY_COLOR_MAIN);
    screen::draw_tile(xpos, ypos, tile);
    xpos += glyphs::tile::width();
  }
}

void scoring_gui_draw_preview_underlay() {
  const uint32_t topy{g_gui.piece_preview.y};
  const uint32_t leftx{g_gui.piece_preview.x};
  const uint32_t rightx{leftx + g_gui.piece_preview_width};
  const uint32_t boty{topy + g_gui.piece_preview_width};

  /* bot row and right column */
  scoring_gui_draw_rectangle_primitive(topy, leftx, boty, rightx, WHITE, 1);
  /* top row */
  scoring_gui_draw_rectangle_primitive(topy, leftx, topy + 1, rightx, DRKGRY,
                                       0);
  /* left column */
  scoring_gui_draw_rectangle_primitive(topy, leftx, boty, leftx + 1, DRKGRY, 0);
}

void init_gui() {
  scoring_gui_draw_underlay();
  scoring_gui_draw_preview_underlay();
  scoring_gui_write_text("Score:", g_gui.line_score_text.y);
  scoring_gui_write_text("Level:",
                         g_gui.line_score_text.y + glyphs::tile::height() * 2);
  scoring_gui_write_text("Lines:",
                         g_gui.line_score_text.y + glyphs::tile::height() * 4);
}

void scoring_gui_draw_bcd_number(const auto &digits, uint32_t yoffset) {
  screen::letter_4bpp_array_t buffer;
  const screen::Tile tile{.side_length = glyphs::tile::width(),
                          .format = VIDEO_FORMAT,
                          .data = std::data(buffer)};

  bool written{false};
  const auto ypos{yoffset};
  auto xpos{g_gui.line_score_text.x};
  for (uint32_t idx{0}; idx < std::size(digits) - 1; ++idx) {
    if (digits[idx] > 0 || written) {
      screen::get_letter_data_4bpp(buffer,
                                   static_cast<char>(digits[idx] + 0x30),
                                   GUI_TEXT_COLOR, GUI_UNDERLAY_COLOR_MAIN);
      screen::draw_tile(xpos, ypos, tile);
      written = true;
    }
    xpos += glyphs::tile::width();
  }
  screen::get_letter_data_4bpp(buffer, static_cast<char>(digits.back() + 0x30),
                               GUI_TEXT_COLOR, GUI_UNDERLAY_COLOR_MAIN);
  screen::draw_tile(xpos, ypos, tile);
}

void draw_level_score() noexcept {
  const auto digits{screen::bcd<5>(g_level + 1)};
  const auto ypos{g_gui.line_score_text.y + glyphs::tile::height() * 3};
  scoring_gui_draw_bcd_number(digits, ypos);
}

void draw_points_score() noexcept {
  const auto digits{screen::bcd<5>(g_points_scored)};
  const auto ypos{g_gui.line_score_text.y + glyphs::tile::height()};
  scoring_gui_draw_bcd_number(digits, ypos);
}

void draw_line_score() noexcept {
  const auto digits{screen::bcd<5>(g_lines_scored)};
  const auto ypos{g_gui.line_score_text.y + glyphs::tile::height() * 5};
  scoring_gui_draw_bcd_number(digits, ypos);
}

void process_level_change(uint32_t lines) noexcept {
  g_points_scored += lines * (g_level + 1) * 5;
  g_lines_scored += lines;
  draw_line_score();
  draw_points_score();
  if (g_lines_scored > g_level_thresholds[g_level]) {
    if (g_level < std::size(g_level_thresholds)) {
      ++g_level;
      g_game_tick_us -= 100'000; /* 100 ms */
      // change_background_color(g_level_color[g_level]);
      change_background_color(g_level);
      // draw_level_score();
    }
  }
}

/* ========================================================================== */
/*          _____ _____ _____ ____  ___ __  __ ___ _   _  ___                 */
/*         |_   _| ____|_   _|  _ \|_ _|  \/  |_ _| \ | |/ _ \                */
/*           | | |  _|   | | | |_) || || |\/| || ||  \| | | | |               */
/*           | | | |___  | | |  _ < | || |  | || || |\  | |_| |               */
/*           |_| |_____| |_| |_| \_\___|_|  |_|___|_| \_|\___/                */
/*                                                                            */
/* ========================================================================== */
[[nodiscard]] bool check_for_collision(Tetrimino blocks,
                                       Location proposed) noexcept {
  /* we collide with the sides and bottom border, or already commited tetriminos
   */

  auto &&check_for_wall{[](const Tetrimino tetri, const Location loc) {
    const auto data{tetri.data()};
    bool result{false};
    for (int bit_position = 0; bit_position < 0xC; ++bit_position) {
      const auto [x, y]{get_position(bit_position)};
      const auto tetrimino_present{(data >> bit_position) & 0b1};
      const auto out_of_bounds{loc.x + x >= PLAY_NO_COLS ||
                               loc.y + y >= PLAY_NO_ROWS};
      const auto tile_idx{g_playfield.get(loc.x + x, loc.y + y)};
      const auto old_block_exists{tile_idx != to_index(Tetrimino::Code::BLANK)};
      /* TODO we can also check if the playfield bit is set. */
      if ((tetrimino_present && out_of_bounds) ||
          (tetrimino_present && old_block_exists)) {
        result = true;
        break;
      }
    }
    return result;
  }};

  return check_for_wall(blocks, proposed);
}

[[nodiscard]] Tetrimino get_random_tetrimino() noexcept {
  while (true) {
    uint32_t rno{get_rand_32()};
    for (uint32_t idx{0}; idx < sizeof(uint32_t) * 8 / 3; idx++) {
      if ((rno & 0b111) != 0) {
        return Tetrimino{to_tetrimino_code(rno & 0b111)};
      }
      rno >>= 3;
    }
  }
}

[[nodiscard]] bool start_a_new_tetrimino() noexcept {
  /* pick a random one
   * it's start location is always x=8, y=0
   * here is where we'll check if game is over, man
   */

  auto &&get_value{[]() {
    while (true) {
      uint32_t rno{get_rand_32()};
      for (uint32_t idx{0}; idx < sizeof(uint32_t) * 8 / 3; idx++) {
        if ((rno & 0b111) != 0) {
          return to_tetrimino_code(rno & 0b111);
        }
        rno >>= 3;
      }
    }
  }};

  g_is_active = true;
  g_pending_commit = true;
  g_tetrimino = g_next_tetrimino;
  g_next_tetrimino = get_random_tetrimino();
  g_prvtetrimino = g_tetrimino;
  g_prvlocation.x = 2;
  g_prvlocation.y = 0;
  g_location.x = 2;
  g_location.y = 0;
  return check_for_collision(g_tetrimino, g_location);
}

void rotate_tetrimino() noexcept {
  auto copy{g_tetrimino};
  copy.rotate();
  if (!check_for_collision(copy, g_location)) {
    g_tetrimino = copy;
  }
}
void move_left() noexcept {
  if (!check_for_collision(g_tetrimino,
                           {.x = g_location.x - 1, .y = g_location.y})) {
    --g_location.x;
  }
}
void move_right() noexcept {
  if (!check_for_collision(g_tetrimino,
                           {.x = g_location.x + 1, .y = g_location.y})) {
    ++g_location.x;
  }
}
void move_down() noexcept {
  uint32_t start_y = g_location.y;
  for (; start_y < PLAY_NO_ROWS; ++start_y) {
    if (check_for_collision(g_tetrimino, {.x = g_location.x, .y = start_y})) {
      break;
    }
  }
  g_location.y = start_y - 1;
  g_is_active = false;
}
void drop_tetrimino() {
  if (check_for_collision(g_tetrimino,
                          {.x = g_location.x, .y = g_location.y + 1})) {
    g_is_active = false;
    return;
  }
  ++g_location.y;
}

void update_prv_tetrimino() { g_prvtetrimino = g_tetrimino; }

void process_tetrimino(UserInput action) {
  switch (action) {
  case UserInput::DROP:
    update_prv_tetrimino();
    drop_tetrimino();
    draw_tetrimino();
    break;
  case UserInput::LEFT:
    update_prv_tetrimino();
    move_left();
    draw_tetrimino();
    break;
  case UserInput::RIGHT:
    update_prv_tetrimino();
    move_right();
    draw_tetrimino();
    break;
  case UserInput::DOWN:
    update_prv_tetrimino();
    move_down();
    draw_tetrimino();
    break;
  case UserInput::UP:
    update_prv_tetrimino();
    rotate_tetrimino();
    draw_tetrimino();
    break;
  default:
    break;
  }
}

void commit_tetrimino() {
  g_pending_commit = false;

  const auto tile_idx{to_index(g_tetrimino.code())};
  const auto bits{g_tetrimino.data()};
  for (int bit_position = 0; bit_position < 0xC; ++bit_position) {
    const auto [x, y]{get_position(bit_position)};
    if ((bits >> bit_position) & 0b1) {
      g_playfield.set(tile_idx, g_location.x + x, g_location.y + y);
#ifdef PRINT_DEBUG_MSG
      printf("setting %d to location {.x = %d, .y = %d}\n", tile_idx,
             g_location.x + x, g_location.y + y);
      printf("  %d\n", g_playfield.get(g_location.x + x, g_location.y + y));
#endif
    }
  }
#ifdef PRINT_DEBUG_MSG
  printf("tile_idx: %d\n", tile_idx);
  printf("bits: 0x%02x\n", bits);
  print_playfield();
#endif
}

/* ========================================================================== */
/*                          ____  _   _ _   _                                 */
/*                         |  _ \| | | | \ | |                                */
/*                         | |_) | | | |  \| |                                */
/*                         |  _ <| |_| | |\  |                                */
/*                         |_| \_\\___/|_| \_|                                */
/*                                                                            */
/* ========================================================================== */
/** @brief Fill the screen with a solid color.
 *  This is a primitive that needs to move to a different module.
 */
void fill_screen_with_background_tile(uint8_t level) noexcept {
  const auto dims{screen::get_virtual_screen_size()};
#if 1
  if (level < std::size(BACKGROUND_TILES)) {
    const auto &tile{BACKGROUND_TILES[level]};
    for (uint32_t yy = 0; yy < dims.height; yy += tile.side_length) {
      for (uint32_t xx = 0; xx < dims.width; xx += tile.side_length) {
        screen::draw_tile(xx, yy, tile);
      }
    }
  }
#else // the old "fill screen with solid color" approach
  /* seems to take forever, which is unsurprising... let's try something else */
  uint8_t *buf{screen::get_video_buffer()};
  for (uint32_t idx = 0, limit = dims.height * dims.width; idx < limit;
       idx += 2) {
    buf[idx >> 1] = (palette_index << 4) | palette_index;
  }
#endif
}
void change_background_color(const uint8_t level) noexcept {
  /* background is anywhere where the playfield and score gui are not
    probably easiest to just iterate through the image

    our steps are:
      draw background tiles
      draw scoring underlays
      draw scoring text and scores
      draw playfield
  */
  // fill_screen_with_color(level);
  fill_screen_with_background_tile(level);
  init_gui();
  draw_level_score();
  draw_line_score();
  draw_points_score();
  draw_tetrimino();
  draw_playfield_border();
  draw_playfield();
}

void init_playfield() noexcept {
  draw_playfield_border();
  for (uint32_t yy = 0; yy < g_play_cfg.grid_height; ++yy) {
    for (uint32_t xx = 0; xx < g_play_cfg.grid_width; ++xx) {
      draw_tetrimino_tile(to_index(Tetrimino::Code::BLANK), xx, yy);
    }
  }
}

void init_play_config() {

  const auto dims{screen::get_virtual_screen_size()};

  /* we need to divide the width up evenly between the playfield and the scoring
   * gui box
   *
   * So, how does that shake out?
   *  playfield (SQUARE_SIZE * 10)
   *  score gui (4*SQUARE_SIZE + margin)
   */
  const auto score_gui_width{
      GUI_OUTSIDE_BORDER_WIDTH * 2 + 4 * SQUARE_SIZE + 2 +
      2 * GUI_PREVIEW_BORDER +
      5 * 2}; // our fine-tuned margin is 5 pixels on either side, I guess
  const auto playfied_width{PLAY_NO_COLS * SQUARE_SIZE};
  const auto remainder_width{dims.width - (score_gui_width + playfied_width)};

  g_play_cfg.grid_height = PLAY_NO_ROWS;
  g_play_cfg.grid_width = PLAY_NO_COLS;
  g_play_cfg.xdimension.off = 2 * remainder_width / 3 + score_gui_width;
  g_play_cfg.ydimension.off = (dims.height - SQUARE_SIZE * PLAY_NO_ROWS) >> 1;
  g_play_cfg.xdimension.scale = SQUARE_SIZE;
  g_play_cfg.ydimension.scale = SQUARE_SIZE;

  /* gui scoreboard box needs to be tall enough to fit:
   *  margin (GUI_OUTSIDE_BORDER_WIDTH)
   *  score (2*glyphs::tile::height())
   *  level (2*glyphs::tile::height())
   *  lines (2*glyphs::tile::height())
   *  piece preview ( 4 * SQUARE_SIZE + margin )
   *  margin (GUI_OUTSIDE_BORDER_WIDTH)
   */
  g_gui.scoring_box_height = 6 * glyphs::tile::height() + 1 +
                             GUI_OUTSIDE_BORDER_WIDTH * 2 + 4 * SQUARE_SIZE +
                             2 + 2 * GUI_PREVIEW_BORDER + 5 * 2;
  g_gui.scoring_box_width = score_gui_width;
  g_gui.scoring_box_start = {.x = remainder_width / 3,
                             .y =
                                 (dims.height - g_gui.scoring_box_height) >> 1};

  g_gui.line_score_text = {
      .x = g_gui.scoring_box_start.x + GUI_OUTSIDE_BORDER_WIDTH + 1,
      .y = g_gui.scoring_box_start.y + GUI_OUTSIDE_BORDER_WIDTH + 1};

  g_gui.piece_preview_width = 4 * SQUARE_SIZE + 2 + 2 * GUI_PREVIEW_BORDER;
  g_gui.piece_preview_height = g_gui.piece_preview_width;

  g_gui.piece_preview = {
      .x = g_gui.scoring_box_start.x +
           ((score_gui_width - g_gui.piece_preview_width) >> 1),
      .y = g_gui.scoring_box_start.y + glyphs::tile::height() * 6 + 6};

  // fill_screen_with_color(g_level_color[g_level]);
  fill_screen_with_background_tile(g_level);
  init_playfield();
  init_gui();
}

void init_the_screen() noexcept {
  /* we are a color application */
  screen::set_format(tetris::VIDEO_FORMAT);
  screen::init_clut(tetris::Palette.data(), tetris::Palette.size());

  /* we use a black background */
  screen::clear_screen();
}

void run() {

  /* game loop:
   *
   * on every game tick:
   *   check for complete lines
   *     clear any if so
   *     move blocks above (if any) down that many rows
   *   move the active tetrimino down if one is active
   *     if moving down will intersect, mark active as dead, then restart game
   *     tick
   *   start a new tetrimino if no active tetrimino
   *     check if active tetrimino is above the play area (i.e. row 21) if so,
   *     GAME OVER MAN
   *   as fast as possible: process user input
   *     if left or right, move the active tetrimino
   *     if up, rotate the active tetrimino
   *     if down, move the active tetrimino all the way down, then run the
   *     next game tick
   */
  static constexpr absolute_time_t PROCESS_TICK_US{INPUT_POLL_TICK_US};
  TimeClock process_clock{.last_touch_point = get_absolute_time()};
  TimeClock game_clock{process_clock};
  UserInput status;
  bool keep_going{true};

  reset_all_global_state();
  g_next_tetrimino = get_random_tetrimino();
  init_the_screen();
  init_play_config();
  draw_line_score();
  draw_points_score();
  draw_level_score();

  gamepad::five::init();

  while (process_user_input() != UserInput::NONE) {
    tight_loop_contents();
  }

  while (keep_going) {
    if (get_elapsed(process_clock) > PROCESS_TICK_US) {
      increment(process_clock, PROCESS_TICK_US);
      status = process_user_input();
      if (status == UserInput::QUIT) {
        break;
      }
      if (g_is_active) {
        process_tetrimino(status);
        if (status == UserInput::DOWN && g_pending_commit && !g_is_active) {
          commit_tetrimino();
          const auto lines{check_for_complete_lines()};
          process_level_change(lines);
          game_clock.last_touch_point = get_absolute_time();
        }
      }
    }
    if (get_elapsed(game_clock) > g_game_tick_us) {
      increment(game_clock, g_game_tick_us);
      if (g_is_active) {
        process_tetrimino(UserInput::DROP);
        if (g_pending_commit && !g_is_active) {
          commit_tetrimino();
          const auto lines{check_for_complete_lines()};
          process_level_change(lines);
        }
      } else {
        if (start_a_new_tetrimino()) {
          /* couldn't start a new tetrimino? game over*/
          screen::melt(BLACK);
          break;
        }
        draw_tetrimino();
      }
    }
    sleep_us(PROCESS_TICK_US >> 4);
  }
}
} // namespace tetris