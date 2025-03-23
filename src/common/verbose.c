#include "common/verbose.h"

#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>

#define VERBOSE_KRED "\x1B[31m"
#define VERBOSE_KGRN "\x1B[32m"
#define VERBOSE_RESET "\x1B[0m"

bool Verbose = false;

void setVerbose(bool setting) {
    Verbose = setting;
}

int verbose(const char * restrict format, ...) {
    if( !Verbose )
        return 0;

    va_list args;
    va_start(args, format);
    int ret = vprintf(format, args);
    va_end(args);

    return ret;
}
