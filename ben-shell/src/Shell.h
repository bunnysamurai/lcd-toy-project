#if !defined(SHELL_H)
#define SHELL_H

#include <stddef.h>
#include <stdio.h>

typedef int (*shell_func_t)(int, const char *[]);
typedef int (*fn_fputc_t)(int, FILE*);

typedef struct {
  const char *id;
  shell_func_t callback;
} ShellFunction_t;

typedef struct {
  fn_fputc_t fn_fputc;
} ShellInterface_t;

#ifdef __cplusplus
extern "C" {
#endif

int Shell_RegisterCommand(ShellFunction_t command);
void Shell_RegisterInterface(ShellInterface_t intf);

/* ShellTask does not return */
void ShellTask(char *shell_buffer, size_t shell_buffer_len, char **argv_buffer,
               size_t argv_len);

#ifdef __cplusplus
}
#endif

#endif