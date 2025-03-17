#ifndef VERBOSE_H_
#define VERBOSE_H_

#include <stdbool.h>

#define VERBOSE_KRED "\x1B[31m"
#define VERBOSE_KGRN "\x1B[32m"
#define VERBOSE_RESET "\x1B[0m"

int verbose(const char * restrict, ...);
void setVerbose(bool);

#endif // VERBOSE_H_
