#include "common/comm_general.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define MAX_BACKLOG 10
#define MAX_PORT 600

typedef struct {
    int server_fd;
    int client_fd;
    bool in_use;
} MockSocketPair;

static MockSocketPair mockPairs[MAX_PORT + 1] = {0};

int tcpCreateSocketWrapper(bool server, char* ipAddress, int port) {
    (void)ipAddress; // Unused in this mock.

    if (port < 0 || port > MAX_PORT) {
    //    verbose("tcpCreateSocketWrapper: Invalid port %d. Valid range is 0 to %d.\n", port, MAX_PORT);
        return -1;
    }

    if (!mockPairs[port].in_use) {
        int sv[2];
        if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) == -1) {
     //       verbose("tcpCreateSocketWrapper: socketpair");
            return -1;
        }
        mockPairs[port].server_fd = sv[0];
        mockPairs[port].client_fd = sv[1];
        mockPairs[port].in_use = true;
    }

    return server ? mockPairs[port].server_fd : mockPairs[port].client_fd;
}

int tcpConnectWrapper(int sockFd, char* ipAddress, int port) {
    (void)ipAddress; // Unused in this mock.

    if (port < 0 || port > MAX_PORT) {
      //  fprintf(stderr, "tcpConnectWrapper: Invalid port %d. Valid range is 0 to %d.\n", port, MAX_PORT);
        return -1;
    }
    if (!mockPairs[port].in_use) {
       // fprintf(stderr, "tcpConnectWrapper: Socket pair for port %d has not been created.\n", port);
        return -1;
    }
    // In this mock, the connection is already established.
    return 0;
}

int tcpAcceptWrapper(int sockFd) {
    // Loop through the socket pairs to find which one matches the given server fd.
    for (int port = 0; port <= MAX_PORT; port++) {
        if (mockPairs[port].in_use && mockPairs[port].server_fd == sockFd) {
            return mockPairs[port].client_fd;
        }
    }
    //fprintf(stderr, "tcpAcceptWrapper: Provided sockFd %d not found in any socket pair.\n", sockFd);
    return -1;
}
