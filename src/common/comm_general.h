#ifndef COMM_GENERAL_H_
#define COMM_GENERAL_H_

#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

int tcpCreateSocketWrapper(bool server, char* ipAddress, int port) {
  // Create TCP socket:
  int socketFd = socket(AF_INET, SOCK_STREAM, 0);

  if (socketFd < 0) {
    return socketFd;
  }

  // invalid information for the server
  if(ipAddress == 0 || port < 0){
    return -1;
  }

  // client does not require bind
  if(!server){
    return socketFd;
  }

  struct sockaddr_in addr;
  (void)memset(&addr,0,sizeof(addr));
  // Set port and IP:
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = inet_addr(ipAddress);

  // Bind to the set port and IP:
  int bindRes = bind(socketFd, (struct sockaddr*)&addr, sizeof(addr));
  if (bindRes < 0) {
    return -1;
  }

  return socketFd;
}

int tcpConnectWrapper(int sockFd, char* ipAddress, int port){
  struct sockaddr_in addr;
  (void)memset(&addr,0,sizeof(addr));

  // Set port and IP:
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = inet_addr(ipAddress);

  int resConnect = connect(sockFd, (struct sockaddr *) &addr, sizeof(addr));

  return resConnect;
}

int tcpAcceptWrapper(int sockFd){
  return accept(sockFd,NULL,NULL);
}

#endif // COMM_GENERAL_H_
