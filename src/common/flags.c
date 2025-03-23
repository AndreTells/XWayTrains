#include <string.h>
#include "common/flags.h"

bool get_flag_value(int argc, char *argv[], const char *flag_str,
                           char** flag_value){
    for (int i = 1; i < argc; i++) {
        // Check if the current argument matches the flag
        if (strcmp(argv[i], flag_str) == 0) {
            // Check if there is a value after the flag
            if(flag_value !=NULL){
                (*flag_value) = argv[i + 1];
            }
            return true; // Return the associated value
        }
    }
    return false; // Flag not found
}
