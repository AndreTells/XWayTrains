#include <sys/socket.h>

#include "resource_database_proxy"

int connectSocket();

int main() {
  int socketFd = connectSocket();

  // connect socket
  RessourceDataBaseProxy_t* safeDatabase = initRessourceDatabaseProxy();

  while (1) {
    // read message from socket
    // create thread to answer
    // check availability
    // if error re add to queue
    // award or not resource (create response)
    // return response
  }

  return 0;
}

int connectSocket(char* ipAddress) {
  // Create UDP socket:
  int socketFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

  if (socketFd < 0) {
    return socketFd;
  }

  struct sockaddr_in server_addr;
  // Set port and IP:
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  //  server_addr.sin_addr.s_addr =  INADDR_ANY;
  server_addr.sin_addr.s_addr = inet_addr(ipAddress);

  // Bind to the set port and IP:
  int bindRes =
      bind(socket_fd, (struct sockaddr*)&addr, sizeof(addr)) if (bindRes < 0) {
    printf("Couldn't bind to the port\n");
    return -1;
  }
}
