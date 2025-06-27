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

static ShellInterface_t s_intf = {.fn_fputc = fputc};

int fputc(int ch, FILE* stream)
{
    s_intf.fn_fputc(ch, stream);
}

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
    printf("  %s\n", shell_ftable[ii].id);
  }
  return 0;
}
static int Shell_Clear(int, const char *[]) {
  /* https://stackoverflow.com/questions/37774983/clearing-the-screen-by-printing-a-character
   */
  /* appears to work for Tera Term and vscode terminals */
  printf("\033[2J"); // clear the screen buffer
  printf("\033[H");  // move cursor to the "home" position
  return 0;
}

#ifdef BUILD_TEST_CMD
static int Shell_Test(int argc, const char *argv[]) {
  printf("Welcome to SHELL_TEST (TM), we just echo back what you give us!\n");
  printf("  argc = %d\n  Arguments = { ", argc);
  for (int ii = 0; ii < argc; ++ii) {
    printf("%s ", argv[ii]);
  }
  printf("}\n");
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
      printf("Impossible case inside shell_populate_argv!\n");
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

  printf("Command '%s' not found!\n", argv[0]);
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

void Shell_RegisterInterface(ShellInterface_t intf){
    s_intf.fn_fputc = intf.fn_fputc;
}

void ShellTask(char *shell_buffer, size_t shell_buffer_len, char **argv_buffer,
               size_t argv_len) {
  if (!(shell_ftable_length < SHELL_FTABLE_LENGTH_MAX)) {
    printf("[FATAL] Shell has been misconfigured.  Please speak with the "
           "developer to get this fixed.\n");
    return;
  }
  printf("%s\n", SHELL_WELCOME);
  for (;;) {
    unsigned int char_count = 0;
    memset(&shell_buffer[0], 0, sizeof(char) * shell_buffer_len);
    memset(&argv_buffer[0], 0, sizeof(char *) * argv_len);

    printf("%s", SHELL_PROMPT);
    // fflush(stdout);
    for (;;) {
      int value = getc(stdin);
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
        putc(ch, stdout);
      } else if (ch == '\n') {
        /* hmm, this seems to be terminal dependent? */
        printf("\r\n");
        break;
      } else if (ch == '\b') {
        if (char_count) {
          /* hmm, this seems to be terminal dependent? */
          putc('\b', stdout);
          putc(' ', stdout);
          putc('\b', stdout);
          char_count--;
          shell_buffer[char_count] = '\0';
        }
      } else {
        /* doing nothing on purpose */
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
      printf("Error! Status = %d\n", status);
    }
  }
}