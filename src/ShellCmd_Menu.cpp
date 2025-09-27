#include "ShellCmd_Menu.hpp"

#include <array>
#include <cstdint>
#include <cstring>
#include <utility>

#include "pico/time.h"

#include "Cursor.hpp"
#include "demo.hpp"
#include "gamepad/gamepad.hpp"
#include "screen/screen.hpp"
#include "snake/snake.hpp"
#include "tetris/tetris.hpp"

namespace {

enum struct UserInstruction {
  NOACTION,
  CURSOR_UP,
  CURSOR_DOWN,
  LAUNCH_PROGRAM
};

struct MenuConfig {
  uint32_t startcol;
  uint32_t startline;
  uint32_t row_spacing;
  uint32_t titlestartline;
};

static constexpr auto TITLE{"----> MevenBoy <----"};

MenuConfig g_cfg{
    .startcol = 1, .startline = 3, .row_spacing = 2, .titlestartline = 1};

bool g_keep_running{true};
std::array<std::pair<const char *, void (*)(void)>, 4> g_f_table{
    std::make_pair("Rattler Race",
                   [] {
                     snake::run();
                     sleep_ms(100);
                   }),
    std::make_pair("Tetris",
                   [] {
                     tetris::run();
                     sleep_ms(100);
                   }),

    std::make_pair("Screen Saver", [] { demo::run_color_rando_art(); }),
    std::make_pair("Exit to Shell", [] {
      g_keep_running = false;
    }) /* keep this one at the bottom */};
static constexpr uint32_t MENU_LIMIT{g_f_table.size() - 1};
Cursor<uint32_t> g_cursor{MENU_LIMIT};
static uint32_t g_prev_cursor{MENU_LIMIT};

void init_menu_cfg() noexcept {
  g_cfg.startcol = 1;
  g_cfg.startline = 3;
  g_cfg.row_spacing = 2;
  g_cfg.titlestartline = 1;
}

[[nodiscard]] UserInstruction process_user_input() noexcept {
  UserInstruction result{UserInstruction::NOACTION};
  auto state{gamepad::five::get()};

  if (state.up) {
    result = UserInstruction::CURSOR_UP;
  }
  if (state.down) {
    result = UserInstruction::CURSOR_DOWN;
  }
  if (state.etc) {
    result = UserInstruction::LAUNCH_PROGRAM;
  }

  if (result == UserInstruction::NOACTION) {
    return result;
  }

  /* in case a button is still pressed, spin until they are all clear */
  bool triggered{false};
  for (;;) {
    state = gamepad::five::get();

    uint8_t data;
    std::memcpy(&data, &state, sizeof(state));
    if (!data && triggered == false) {
      triggered = true;
    } else if (data && triggered == true) {
      triggered = false;
    } else if (!data && triggered == true) {
      break;
    }
    sleep_ms(10);
  }

  return result;
}

void draw_menu() noexcept {

  /* ----> MevenBoy <---- */
  {
    uint32_t ypos{g_cfg.titlestartline};
    uint32_t xpos{g_cfg.startcol};
    for (uint32_t idx = 0; TITLE[idx] != '\0'; ++idx) {
      screen::draw_letter(xpos, ypos, TITLE[idx]);
      xpos++;
    }
  }

  uint32_t ypos{g_cfg.startline};
  for (uint32_t idx = 0; idx < MENU_LIMIT; ++idx) {

    // for (const auto pairelem : g_f_table) {
    const auto pairelem{g_f_table[idx]};
    uint32_t xpos{g_cfg.startcol};
    screen::draw_letter(xpos, ypos, '[');
    xpos++;
    screen::draw_letter(xpos, ypos, ' ');
    xpos++;
    screen::draw_letter(xpos, ypos, ']');
    xpos++;
    screen::draw_letter(xpos, ypos, ' ');
    xpos++;

    const char *c_str{pairelem.first};
    for (uint32_t idx = 0; c_str[idx] != '\0'; ++idx) {
      screen::draw_letter(xpos, ypos, c_str[idx]);
      xpos++;
    }
    ypos += g_cfg.row_spacing;
  }
}

void update_selection() noexcept {
  if (g_prev_cursor != g_cursor) {
    /* actually update the screen*/
    const uint32_t xpos{g_cfg.startcol};
    uint32_t ypos{g_cfg.startline};
    for (uint32_t idx = 0; idx < MENU_LIMIT; ++idx) {
      // for (const auto _ : g_f_table) {
      screen::draw_letter(xpos, ypos, '[');
      screen::draw_letter(xpos + 1, ypos, ' ');
      ypos += g_cfg.row_spacing;
    }

    ypos = g_cfg.startline + g_cfg.row_spacing * g_cursor;
    screen::draw_letter(xpos, ypos, '[');
    screen::draw_letter(xpos + 1, ypos, '*');

    g_prev_cursor = g_cursor;
  }
}

} // namespace

int ShellCmd_Menu(int, const char *[]) noexcept {

  /* uses 5-pad */
  gamepad::five::init();

  /* stay in console mode */
  if (!screen::set_console_mode()) {
    return -1;
  }

  /* initialize the menu configuration */
  init_menu_cfg();
  g_keep_running = true;
  bool wait_for_noaction{true};
  while (g_keep_running) {

    g_prev_cursor = MENU_LIMIT;
    g_cursor.reset();

    if (!screen::set_console_mode()) {
      return -1;
    }
    draw_menu();

    bool in_menu{true};
    while (in_menu) {

      /* draw the menu */
      update_selection();

      /* read user input
       * if "up" or "down", move selection cursor
       * if "etc", launch the registered program
       * otherwise, ignore
       *
       */
      const UserInstruction input{process_user_input()};
      switch (input) {
      case UserInstruction::CURSOR_UP:
        if (!wait_for_noaction) {
          g_cursor--;
          wait_for_noaction = true;
        }
        break;
      case UserInstruction::CURSOR_DOWN:
        if (!wait_for_noaction) {
          g_cursor++;
          wait_for_noaction = true;
        }
        break;
      case UserInstruction::LAUNCH_PROGRAM:
        if (!wait_for_noaction) {
          g_f_table[g_cursor].second();
          gamepad::five::init();
          in_menu = false;
          wait_for_noaction = true;
        }
        break;
      case UserInstruction::NOACTION:
        wait_for_noaction = false;
        break;
      }
    }
  }

  gamepad::five::deinit();
  screen::clear_console();

  return 0;
}