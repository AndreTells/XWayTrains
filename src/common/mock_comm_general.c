#include "common/comm_general.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>

#define MAX_BACKLOG 10

int tcpCreateSocketWrapper(bool server, char* ipAddress, int port) {
  return 43;
}

int tcpConnectWrapper(int sockFd, char* ipAddress, int port) {
  return 0;
}

int tcpAcceptWrapper(int sockFd) {
  return 44;
}
