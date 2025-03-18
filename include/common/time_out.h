/**
 * @file time_out.h
 * @brief Header file for timeout handling functions.
 *
 * This file defines functions and constants for managing timeouts,
 * including loading timespec structures and waiting on file descriptors.
 */
#ifndef TIME_OUT_H_
#define TIME_OUT_H_

#include <time.h>
/**
 * @brief Timeout duration in seconds.
 */
#define WAIT_TIMEOUT 5

/**
 * @brief Loads the current time into a timespec structure.
 *
 * This function initializes a timespec structure with the current system time.
 *
 * @param ts Pointer to a timespec structure to be loaded.
 * @return 0 on success, or a negative value on failure.
 */
int loadTimeSpec(struct timespec* ts);

/**
 * @brief Waits for an event on a file descriptor with a timeout.
 *
 * This function waits for activity on the specified file descriptor
 * for a predefined timeout duration.
 *
 * @param fd File descriptor to wait on.
 * @return 0 on success, or a negative value on timeout or error.
 */
int fileDescriptorTimedWait(int fd);

#endif // TIME_OUT_H_
