#include "common/verbose.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

#define VERBOSE_KRED "\x1B[31m"
#define VERBOSE_KGRN "\x1B[32m"
#define VERBOSE_RESET "\x1B[0m"

bool Verbose = false;

void setVerbose(bool setting) { Verbose = setting; }

int verbose(const char* __restrict format, ...) {
  if (!Verbose) return 0;

  va_list args;
  va_start(args, format);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
  int ret = vprintf(format, args);
#pragma GCC diagnostic pop
  va_end(args);

  return ret;
}
