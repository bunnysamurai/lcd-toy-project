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

int mywrap_putchar(int c, FILE *) { return stdio_putchar(c); }
int mywrap_flush(FILE *) {
  stdio_flush();
  return 0;
}
int mywrap_getchar(FILE *) { return stdio_getchar(); }

int ShellCmd_Demo(int argc, const char *argv[]) {
  multicore_launch_core1([] { demo::run_animation(); });
  while (EOF == stdio_getchar()) {
    /* spin */
  }
  multicore_reset_core1();
  bsio::clear_screen();
  return 0;
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
  ShellCmd_Demo(0, nullptr);

  static constexpr uint SHELL_BUFFER_LEN{64U};
  static constexpr uint ARGV_LEN{32U};
  static char shell_buffer[SHELL_BUFFER_LEN]; /* characters input by the user */
  static char
      *argument_values[ARGV_LEN]; /* list of pointers into shell_buffer */

  ShellFunction_t additional_cmds[] = {
      {.id = "clear", .callback = ShellCmd_Clear},
      {.id = "demo", .callback = ShellCmd_Demo},
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