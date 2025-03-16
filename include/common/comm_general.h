#ifndef COMM_GENERAL_H_
#define COMM_GENERAL_H_

#include <stdbool.h>

int tcpCreateSocketWrapper(bool server, char* ipAddress, int port);

int tcpConnectWrapper(int sockFd, char* ipAddress, int port);

int tcpAcceptWrapper(int sockFd);

#endif  // COMM_GENERAL_H_
