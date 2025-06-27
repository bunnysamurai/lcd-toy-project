#include "Shell.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define BUILD_TEST_CMD

static const char *SHELL_PROMPT = "bs:> ";
static const char *SHELL_WELCOME = "========================================\n"
                                   "               bs (Ben Shell)           \n"
                                   "========================================\n"
                                   "Type 'help' to see list of available    \n"
                                   "commands.                               ";

// clang-format off
static ShellInterface_t s_intf = {
  .printf = printf,
  .putc = putc,
  .flush = fflush,
  .getc = getc
};
// clang-format on

static int Shell_Help(int, const char *[]);
static int Shell_Clear(int, const char *[]);
#ifdef BUILD_TEST_CMD
static int Shell_Test(int argc, const char *argv[]);
#endif

#define SHELL_FTABLE_LENGTH_MAX                                                \
  32 /* Maximum number of commands the shell can support */
static ShellFunction_t shell_ftable[SHELL_FTABLE_LENGTH_MAX] = {
    {.id = "help", .callback = Shell_Help},
    {.id = "clear", .callback = Shell_Clear},
#ifdef BUILD_TEST_CMD
    {.id = "test", .callback = Shell_Test},
#endif
};
#ifdef BUILD_TEST_CMD
#define BUILD_CFG_TEST_COMMANDS 1
#else
#define BUILD_CFG_TEST_COMMANDS 0
#endif
static size_t shell_ftable_length = 2 + BUILD_CFG_TEST_COMMANDS;

static int Shell_Help(int, const char *[]) {
  for (int ii = 0; ii < shell_ftable_length; ++ii) {
    s_intf.printf("  %s\n", shell_ftable[ii].id);
  }
  return 0;
}
static int Shell_Clear(int, const char *[]) {
  /* https://stackoverflow.com/questions/37774983/clearing-the-screen-by-printing-a-character
   */
  /* appears to work for Tera Term and vscode terminals */
  s_intf.printf("\033[2J"); // clear the screen buffer
  s_intf.printf("\033[H");  // move cursor to the "home" position
  return 0;
}

#ifdef BUILD_TEST_CMD
static int Shell_Test(int argc, const char *argv[]) {
  s_intf.printf(
      "Welcome to SHELL_TEST (TM), we just echo back what you give us!\n");
  s_intf.printf("  argc = %d\n  Arguments = { ", argc);
  for (int ii = 0; ii < argc; ++ii) {
    s_intf.printf("%s ", argv[ii]);
  }
  s_intf.printf("}\n");
  return 0;
}
#endif

static void shell_convert_whitespace_to_null(char *data, int length) {
  for (int ii = 0; ii < length; ++ii) {
    if (isblank(data[ii])) {
      data[ii] = '\0';
    }
  }
}
static void shell_populate_argv(char *shell_buffer, int length,
                                char **arg_values, int *arg_count) {
  /* check for degenerate edge case */
  if (shell_buffer[0] == '\0') {
    return;
  }

  char *cur_pointer = shell_buffer;
  *arg_count = 0;

  int state = 0; /* 0 - search for \0, 1 - search for not \0 */
  for (int ii = 0; ii < length; ++ii) {
    switch (state) {
    case 0: /*0 - search for \0 */
    {
      if (shell_buffer[ii] == '\0') {
        arg_values[*arg_count] = cur_pointer;
        ++(*arg_count);
        state = 1;
      }
      break;
    }
    case 1: /* 1 - search for not \0 */
    {
      if (shell_buffer[ii] != '\0') {
        cur_pointer = &shell_buffer[ii];
        state = 0;
      }
      break;
    }
    default: {
      /* shouldn't be possible */
      s_intf.printf("Impossible case inside shell_populate_argv!\n");
      break;
    }
    }
  }
}
static int call_shell_function(const char **argv, int argc) {
  /* check for degenerate edge cases */
  if (argc == 0) {
    return 0;
  }

  /* search for an ID that matches argv[0] */
  for (int ii = 0; ii < shell_ftable_length; ++ii) {
    if (strcmp(argv[0], shell_ftable[ii].id) == 0) {
      return shell_ftable[ii].callback(argc, argv);
    }
  }

  s_intf.printf("Command '%s' not found!\n", argv[0]);
  return 0;
}

int Shell_RegisterCommand(ShellFunction_t command) {
  if (shell_ftable_length < SHELL_FTABLE_LENGTH_MAX) {
    shell_ftable[shell_ftable_length++] = command;
    return 0;
  }
  /* Shell ftable is full.  This is a development-level issue. */
  return 1;
}

void Shell_RegisterInterface(ShellInterface_t intf) {
  s_intf.printf = intf.printf;
  s_intf.putc = intf.putc;
  s_intf.flush = intf.flush;
  s_intf.getc = intf.getc;
}

void ShellTask(char *shell_buffer, size_t shell_buffer_len, char **argv_buffer,
               size_t argv_len) {
  if (!(shell_ftable_length < SHELL_FTABLE_LENGTH_MAX)) {
    s_intf.printf(
        "[FATAL] Shell has been misconfigured.  Please speak with the "
        "developer to get this fixed.\n");
    return;
  }
  s_intf.printf("%s\n", SHELL_WELCOME);
  for (;;) {
    unsigned int char_count = 0;
    memset(&shell_buffer[0], 0, sizeof(char) * shell_buffer_len);
    memset(&argv_buffer[0], 0, sizeof(char *) * argv_len);

    s_intf.printf("%s", SHELL_PROMPT);
    s_intf.flush(stdout);
    for (;;) {
      int value = s_intf.getc(stdin);
      if (value == EOF) {
        continue;
      }

      char ch = (char)value;
      /* if ch is a visible character (i.e. not a control seqeunce), then
       *    stuff it into shell_buffer
       *    echo it back to the user
       * otherwise, if ch is 'enter', then
       *    break from the while loop
       * otherwise, if ch is 'backspace', then
       *    carriage return
       *    overwrite line with blanks
       *    reprint shell_buffer with one less character
       * otherwise,
       *    do nothing
       */
      if (isprint(ch)) {
        shell_buffer[char_count++] = ch;
        char_count &= (shell_buffer_len - 1) & 0xFFU;
        /* hmm, this seems to be terminal dependent? */
        s_intf.putc(ch, stdout);
      } else if (ch == '\n' || ch == '\r') {
        /* hmm, this seems to be terminal dependent? */
        // s_intf.printf("\r\n");
        s_intf.putc('\n', stdout);
        break;
      } else if (ch == '\b') {
        if (char_count) {
          /* hmm, this seems to be terminal dependent? */
          s_intf.putc('\b', stdout);
          s_intf.putc(' ', stdout);
          s_intf.putc('\b', stdout);
          char_count--;
          shell_buffer[char_count] = '\0';
        }
      } else {
        /* doing nothing on purpose */
        s_intf.printf("hey, got a character I don't understand! It's hex value is 0x%02X\n", ch);
      }
    }
    /*
     * All whitespace is replaced with /0
     * Call function whose ID matches arg[0]
     *   If no match, issue error and move on
     * Create argv, then call the match
     * That's it
     */
    int argument_count = 0;
    shell_convert_whitespace_to_null(&shell_buffer[0], shell_buffer_len);
    shell_populate_argv(&shell_buffer[0], shell_buffer_len, &argv_buffer[0],
                        &argument_count);
    int status =
        call_shell_function((const char **)&argv_buffer[0], argument_count);
    if (status) {
      s_intf.printf("Error! Status = %d\n", status);
    }
  }
}