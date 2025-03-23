#ifndef FLAGS_H_
#define FLAGS_H_

#include <stdbool.h>

#define VERBOSE_FLAG "-v"
#define IP_ADDRESS_FLAG "-ip"

bool get_flag_value(int argc, char *argv[], const char *flag_str,
                           char** flag_value);

#endif // FLAGS_H_
