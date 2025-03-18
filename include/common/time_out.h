#ifndef TIME_OUT_H_
#define TIME_OUT_H_

#include <time.h>

#define WAIT_TIMEOUT 5

int loadTimeSpec(struct timespec* ts);

int fileDescriptorTimedWait(int fd);

#endif // TIME_OUT_H_
