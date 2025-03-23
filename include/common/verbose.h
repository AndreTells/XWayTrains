/**
 * @file verbose.h
 * @brief Header file for verbose logging functionality.
 *
 * This file provides functions for controlling and using verbose logging,
 * including formatted output with color support.
 */
#ifndef VERBOSE_H_
#define VERBOSE_H_

#include <stdbool.h>

/**
 * @brief ANSI escape code for colors
 */
#define VERBOSE_KRED "\x1B[31m"
#define VERBOSE_KGRN "\x1B[32m"
#define VERBOSE_RESET "\x1B[0m"

/**
 * @brief Prints verbose messages if verbose mode is enabled.
 *
 * This function provides formatted output similar to printf but respects
 * the verbosity setting.
 *
 * @param format Format string (printf-style).
 * @param ... Additional arguments corresponding to the format string.
 * @return The number of characters printed, or a negative value on error.
 */
int verbose(const char * restrict, ...);

/**
 * @brief Prints verbose messages if verbose mode is enabled.
 *
 * This function provides formatted output similar to printf but respects
 * the verbosity setting.
 *
 * @param format Format string (printf-style).
 * @param ... Additional arguments corresponding to the format string.
 * @return The number of characters printed, or a negative value on error.
 */
void setVerbose(bool);

#endif // VERBOSE_H_
