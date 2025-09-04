#include <algorithm>
#include <array>
#include <cstring>
#include <limits>
#include <string>

#include "pico/multicore.h"
#include "pico/printf.h"
#include "pico/stdlib.h"

#include "Shell.h"
#include "ShellCmd_Menu.hpp"
#include "bsio.hpp"
#include "demo.hpp"
#include "screen/screen.hpp"
#include "snake/snake.hpp"
#include "status_utilities.hpp"

int mywrap_putchar(int c, FILE *) { return stdio_putchar(c); }
int mywrap_flush(FILE *) {
  stdio_flush();
  return 0;
}
int mywrap_getchar(FILE *) { return stdio_getchar(); }

static void shellcmd_screen_usage(const auto &cmd) {
  /* clang-format off */
      // printf("  %s [format | size | buflen | clear | fill | calibrate | touch]\n\n", cmd);
      static constexpr const char* usage_str{ R"(
screen format [ FORMAT ]
  Prints the screen's current BPP format specifier.
  Can be one of 
    {GREY1, GREY2, GREY4, 
     RGB565_LUT8, RGB565}
  If FORMAT is given, sets the screen's BPP format.
  Valid arguments are {1, 2, 4, 8, 16}

screen size
  Prints the width and height of the dispaly, in 
  pixels

screen buflen
  Maximum size of the video buffer, in bytes

screen clear
  Sets the display to "black"

screen fill VALUE
  Fills the video buffer with the raw value VALUE

screen calibrate
  Runs a touch-screen calibration routine.
  Objective is to map touch samples values to pixel
  position on the screen.

screen touch [ ZTHRESH FIRST_TOSS LAST_TOSS ]
  Prints the current touch configuration.
  If the arguments are given, instead configures 
  the touch screen.  Parameters are:
  ZTHRESH - range [0,~4000], higher values require
            a lighter(?) press to start the
            sampling process
  FIRST_TOSS - integer, number of samples to throw
                away before determing if this press
                is intentional
  LAST_TOSS - integer, number of samples to average
              together to determine where the touch
              location is on the screen.  These
              samples are only taken after 
              FIRST_TOUCH number of samples have 
              been taken.
)"};
  /* clang-format on */
  printf("%s\n", usage_str);
}
int ShellCmd_Screen(int argc, const char *argv[]) {
  /* help */
  if (argc == 1 || (argc == 2 && !strcmp("help", argv[1]))) {
    shellcmd_screen_usage(argv[0]);
    return 0;
  }
  /* other commands */
  if (argc > 1) {
    if (!strcmp("clear", argv[1])) {
      screen::clear_console();
    }
    if (!strcmp("buflen", argv[1])) {
      printf("%d\n", screen::get_buf_len());
    }
    if (!strcmp("format", argv[1])) {
      if (argc == 2) {
        const auto fmt{screen::get_format()};
        switch (fmt) {
        case screen::Format::GREY1:
          printf("GREY1\n");
          break;
        case screen::Format::GREY2:
          printf("GREY2\n");
          break;
        case screen::Format::GREY4:
          printf("GREY4\n");
          break;
        case screen::Format::RGB565_LUT8:
          printf("RGB565_LUT8\n");
          break;
        case screen::Format::RGB565:
          printf("RGB565\n");
          break;
        }
      } else {
        const auto bpp{std::stoi(argv[2])};
        switch (bpp) {
        case 1:
          screen::set_format(screen::Format::GREY1);
          break;
        case 2:
          screen::set_format(screen::Format::GREY2);
          break;
        case 4:
          screen::set_format(screen::Format::GREY4);
          break;
        case 8:
          screen::set_format(screen::Format::RGB565_LUT8);
          break;
        case 16:
          screen::set_format(screen::Format::RGB565);
          break;
        default:
          printf("Format %d unsupported\n", bpp);
        }
      }
    }
    if (!strcmp("size", argv[1])) {
      const auto dims{screen::get_virtual_screen_size()};
      printf("%dx%d\n", dims.width, dims.height);
    }
    if (!strcmp("calibrate", argv[1])) {
      printf("STUB: run touch screen calibration routine\n");
    }

    if (!strcmp("touch", argv[1])) {
      if (argc == 2) /* print current config*/
      {
        dispTouchCfg_t cfg;
        dispGetTouchConfiguration(&cfg);
        printf("  ZTHRESH { %d }\n  FIRST_TOSS { %d }\n  LAST_TOSS { %d }\n",
               cfg.touch_zthresh, cfg.first_toss, cfg.last_toss);
      }
      if (argc == 5) /* requesting a change */
      {
        const auto zthresh{static_cast<uint32_t>(std::stoi(argv[2]))};
        const auto ftoss{static_cast<uint8_t>(std::stoi(argv[3]))};
        const auto ltoss{static_cast<uint8_t>(std::stoi(argv[4]))};
        dispConfigureTouch(dispTouchCfg_t{
            .touch_zthresh = zthresh, .first_toss = ftoss, .last_toss = ltoss});
      }
    }
  }
  if (argc > 2) {
    if (!strcmp("fill", argv[1])) {
      const auto fill_value{std::stoi(argv[2])};
      screen::fill_screen(fill_value);
    }
  }
  if (argc > 4) {
  }
  return 0;
}

int ShellCmd_Snake(int argc, const char *argv[]) {
  snake::run();
  if (!screen::set_console_mode()) {
    return -1;
  }
  return 0;
}

static demo::TouchConfig s_cfg;
static void run_touch_demo_impl() { demo::run_touch_demo(s_cfg); }

int ShellCmd_Demo(int argc, const char *argv[]) {
  if (argc == 2 && !strcmp("help", argv[1])) {
    printf("%s [text | rando | touch | snake]\n", argv[0]);
    return 0;
  }
  if (argc > 1 && !strcmp("text", argv[1])) {
    multicore_launch_core1([] { demo::run_text_animation(); });
  } else if (argc > 1 && !strcmp("rando", argv[1])) {
    multicore_launch_core1([] { demo::run_color_rando_art(); });
  } else if (argc > 1 && !strcmp("touch", argv[1])) {
    screen::clear_screen();
    if (argc != 5) {
      multicore_launch_core1([] { demo::run_touch_demo(); });
    } else {
      s_cfg = {.touch_poll_interval_ms{10},
               .touchcfg = {.touch_zthresh = atoi(argv[2]),
                            .first_toss = atoi(argv[3]),
                            .last_toss = atoi(argv[4])}};
      multicore_launch_core1(run_touch_demo_impl);
    }
  } else {
    printf("%s [text | rando | touch]\n", argv[0]);
    return 0;
  }
  while (EOF == stdio_getchar()) {
    /* spin */
  }
  multicore_reset_core1();
  if (!screen::set_console_mode()) {
    return -1;
  }
  return 0;
}

static int ShellCmd_Clear(int, const char *[]) {
  screen::clear_console();
  return 0;
}

int main() {
  if (!bsio::init()) {
    BlinkStatus{BlinkStatus::Milliseconds{250}}.blink_forever();
  }

  static constexpr uint SHELL_BUFFER_LEN{64U};
  static constexpr uint ARGV_LEN{32U};
  static char shell_buffer[SHELL_BUFFER_LEN]; /* characters input by the user */
  static char
      *argument_values[ARGV_LEN]; /* list of pointers into shell_buffer */

  {
    ShellFunction_t additional_cmds[] = {
        {.id = "clear", .callback = ShellCmd_Clear},
        {.id = "demo", .callback = ShellCmd_Demo},
        {.id = "screen", .callback = ShellCmd_Screen},
        {.id = "snake", .callback = ShellCmd_Snake},
        {.id = "menu", .callback = ShellCmd_Menu},
    };
    const int ADDITIONAL_CMDS_LENGTH =
        sizeof(additional_cmds) / sizeof(ShellFunction_t);
    for (int ii = 0; ii < ADDITIONAL_CMDS_LENGTH; ++ii) {
      Shell_RegisterCommand(additional_cmds[ii]);
    }
  }

  {
    const ShellInterface_t my_interface{
        .printf = stdio_printf,
        .putc = mywrap_putchar,
        .flush = mywrap_flush,
        .getc = mywrap_getchar,
    };
    Shell_RegisterInterface(my_interface);
  }

  // start by running the demo
  {
    // const char *argvs[2] = {"demo", "rando"};
    // ShellCmd_Demo(2, argvs);
    ShellCmd_Menu(0, nullptr);
  }

  /* launch the shell... does not return */
  ShellTask(&shell_buffer[0], SHELL_BUFFER_LEN, &argument_values[0], ARGV_LEN);
}