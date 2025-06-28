#include <algorithm>
#include <array>
#include <limits>

#include "pico/multicore.h"
#include "pico/printf.h"
#include "pico/stdlib.h"

#include "Shell.h"
#include "bsio.hpp"
#include "demo.hpp"
#include "status_utilities.hpp"

bool stop_demo{false};
void set_stop_demo() {
  // FIXME race conditions
  stop_demo = true;
}
bool get_stop_demo() { return stop_demo; }

int mywrap_putchar(int c, FILE *) { return stdio_putchar(c); }
int mywrap_flush(FILE *) {
  stdio_flush();
  return 0;
}
int mywrap_getchar(FILE *) { return stdio_getchar(); }

int ShellCmd_Info(int argc, const char *argv[]) {
  (void)argc;
  (void)argv;

  printf("Hello, here's some info...\n");
  return EXIT_SUCCESS;
}
static int ShellCmd_Clear(int, const char *[]) {
  bsio::clear_screen();
  return 0;
}

int main() {
  if (!bsio::init()) {
    BlinkStatus{BlinkStatus::Milliseconds{250}}.blink_forever();
  }

  // start by running the demo
  // FIXME race conditions
  multicore_launch_core1([] {
    demo::run_animation(get_stop_demo);
    while (true) {
      /* spin forever */
    }
  });
  while (!stop_demo) {
    while (EOF == stdio_getchar()) {
      /* spin */
    }
    set_stop_demo();
  }
  sleep_ms(
      1000); // TODO hack sleep to wait for the animation to stop running.
             // There's probably a way to check if the other core is halted...

  bsio::clear_screen();

  static constexpr uint SHELL_BUFFER_LEN{64U};
  static constexpr uint ARGV_LEN{32U};
  static char shell_buffer[SHELL_BUFFER_LEN]; /* characters input by the user */
  static char
      *argument_values[ARGV_LEN]; /* list of pointers into shell_buffer */

  ShellFunction_t additional_cmds[] = {
      {.id = "info", .callback = ShellCmd_Info},
      {.id = "clear", .callback = ShellCmd_Clear},
  };
  const int ADDITIONAL_CMDS_LENGTH =
      sizeof(additional_cmds) / sizeof(ShellFunction_t);
  for (int ii = 0; ii < ADDITIONAL_CMDS_LENGTH; ++ii) {
    Shell_RegisterCommand(additional_cmds[ii]);
  }

  const ShellInterface_t my_interface{.printf = stdio_printf,
                                      .putc = mywrap_putchar,
                                      .flush = mywrap_flush,
                                      .getc = mywrap_getchar};
  Shell_RegisterInterface(my_interface);

  /* launch the shell... does not return */
  ShellTask(&shell_buffer[0], SHELL_BUFFER_LEN, &argument_values[0], ARGV_LEN);
}