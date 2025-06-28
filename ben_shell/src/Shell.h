#if !defined(SHELL_H)
#define SHELL_H

#include <stddef.h>
#include <stdio.h>

typedef int (*shell_func_t)(int, const char *[]);
typedef int (*fn_printf_t)(const char*, ...);
typedef int (*fn_putc_t)(int, FILE*);
typedef int (*fn_flush_t)(FILE*);
typedef int (*fn_getc_t)(FILE*);

typedef struct {
  const char *id;
  shell_func_t callback;
} ShellFunction_t;

typedef struct {
  fn_printf_t printf;
  fn_putc_t putc;
  fn_flush_t flush;
  fn_getc_t getc;
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