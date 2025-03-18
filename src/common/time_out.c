#include "common/verbose.h"
#include "common/time_out.h"

#include <stdio.h>
#include <poll.h>
#include <errno.h>
#include <time.h>

int loadTimeSpec(struct timespec* ts){
  // Get the current time
  int ret = clock_gettime(CLOCK_REALTIME, ts);
  if(ret<0){
    return ret;
  }

  ts->tv_sec += WAIT_TIMEOUT;
  return 0;
}

int fileDescriptorTimedWait(int fd){
    struct pollfd fds[1];
    int ret;

    // Set up the pollfd structure
    fds[0].fd = fd;          // Socket file descriptor
    fds[0].events = POLLIN;      // Wait for incoming data

    // Convert timeout_sec to milliseconds for poll()
    int timeout_ms = WAIT_TIMEOUT * 1000;

    // Wait for the socket to become readable
    ret = poll(fds, 1, timeout_ms);

    if (ret == -1) {
        // poll() error
        perror("poll");
        return -1;
    }

    else if (ret == 0) {
        // Timeout occurred
        verbose("[TIMED READ]: Timeout\n");
        errno = ETIMEDOUT;
        return -1;
    }

    return 0;
}
