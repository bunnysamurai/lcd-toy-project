
#include <stdio.h>
#include <stdlib.h>

#include "../src/Shell.h"

#define SHELL_BUFFER_LEN 64U
#define ARGV_LEN 32U
static char shell_buffer[SHELL_BUFFER_LEN]; /* characters input by the user */
static char *argument_values[ARGV_LEN]; /* list of pointers into shell_buffer */

int ShellCmd_Info(int argc, const char *argv[]) {
  (void)argc;
  (void)argv;

  printf("Hello, here's some info...\n");
  return EXIT_SUCCESS;
}

void main(void) {
  /* register application specific commands */
  ShellFunction_t additional_cmds[] = {
      {.id = "info", .callback = ShellCmd_Info},
  };
  const int ADDITIONAL_CMDS_LENGTH =
      sizeof(additional_cmds) / sizeof(ShellFunction_t);
  for (int ii = 0; ii < ADDITIONAL_CMDS_LENGTH; ++ii) {
    Shell_RegisterCommand(additional_cmds[ii]);
  }

  /* launch the shell... does not return */
  ShellTask(&shell_buffer[0], SHELL_BUFFER_LEN, &argument_values[0], ARGV_LEN);
}