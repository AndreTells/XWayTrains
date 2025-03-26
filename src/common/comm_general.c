#include "common/comm_general.h"

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "common/verbose.h"

#define MAX_BACKLOG 10

int tcpCreateSocketWrapper(bool server, char* ipAddress, const uint16_t port) {
  // Create TCP socket:
  int socketFd = socket(AF_INET, SOCK_STREAM, 0);
  verbose("[tcpCreateSocketWrapper] Creating socket %d...\n", socketFd);

  if (socketFd < 0) {
    perror("[tcpCreateSocketWrapper]: socket \n");
    return socketFd;
  }

  // client does not require bind
  if (!server) {
    return socketFd;
  }

  // invalid information for the server
  if (ipAddress == 0) {
    verbose("[tcpCreateSocketWrapper]: " VERBOSE_KRED "fail: invalid IP \n");
    return -1;
  }

  struct sockaddr_in addr;
  (void)memset(&addr, 0, sizeof(addr));
  // Set port and IP:
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = inet_addr(ipAddress);

  // Bind to the set port and IP:
  int bindRes = bind(socketFd, (struct sockaddr*)&addr, sizeof(addr));
  if (bindRes < 0) {
    perror("[tcpCreateSocketWrapper]: fail bind");
    return -1;
  }

  listen(socketFd, MAX_BACKLOG);

  return socketFd;
}

int tcpConnectWrapper(int sockFd, char* ipAddress, const uint16_t port) {
  struct sockaddr_in addr;
  (void)memset(&addr, 0, sizeof(addr));

  // Set port and IP:
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = inet_addr(ipAddress);

  int resConnect = connect(sockFd, (struct sockaddr*)&addr, sizeof(addr));
  if (resConnect == -1) {
    perror("[tcpConnectWrapper]: ");
  }
  return resConnect;
}

int str_to_uint16(const char* str, uint16_t* res) {
  long int val = strtol(str, NULL, 10);
  if (errno == ERANGE || val > UINT16_MAX || val < 0) return -1;
  *res = (uint16_t)val;
  return 0;
}

int tcpAcceptWrapper(int sockFd) { return accept(sockFd, NULL, NULL); }
