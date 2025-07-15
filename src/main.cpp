#include <algorithm>
#include <array>
#include <cstring>
#include <limits>
#include <string>

#include "pico/multicore.h"
#include "pico/printf.h"
#include "pico/stdlib.h"

#include "Shell.h"
#include "bsio.hpp"
#include "demo.hpp"
#include "screen/screen.hpp"
#include "status_utilities.hpp"

int mywrap_putchar(int c, FILE *) { return stdio_putchar(c); }
int mywrap_flush(FILE *) {
  stdio_flush();
  return 0;
}
int mywrap_getchar(FILE *) { return stdio_getchar(); }
// void restore_to_1bpp() {
//   screen::set_format(screen::Format::GREY1);
//   screen::clear_console();
// }

int ShellCmd_Screen(int argc, const char *argv[]) {
  if (argc > 1) {
    if (!strcmp("help", argv[1])) {
      printf("  %s [format | size | buflen | clear | fill]\n", argv[0]);
      printf("screen format FORMAT will set the format\n");
      printf("Valid arguments are {1, 2, 4, 8, 16}\n");
      printf("screen fill U8 will fill the video buffer with this raw value\n");
    }
    if(!strcmp("clear", argv[1])){
      screen::clear_screen();
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
  }
  if(argc > 2)
  {
    if(!strcmp("fill", argv[1]))
    {
      const auto fill_value{ std::stoi(argv[2])};
      screen::fill_screen(fill_value);
    }
  }
  return 0;
}

static demo::TouchConfig s_cfg;
static void run_touch_demo_impl() { demo::run_touch_demo(s_cfg); }

int ShellCmd_Demo(int argc, const char *argv[]) {
  if (argc == 2 && !strcmp("help", argv[1])) {
    printf("%s [text | rando | touch]\n", argv[0]);
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
    const char *argvs[2] = {"demo", "touch"};
    ShellCmd_Demo(2, argvs);
  }

  /* launch the shell... does not return */
  ShellTask(&shell_buffer[0], SHELL_BUFFER_LEN, &argument_values[0], ARGV_LEN);
}