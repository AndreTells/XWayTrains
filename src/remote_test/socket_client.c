#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUF_SIZE 500

int client(char *host, char *port) {
  int sfd, s;
  char buf[BUF_SIZE];
  char output[BUF_SIZE];
  ssize_t len = 0;
  ssize_t nread;
  struct addrinfo hints;
  struct addrinfo *result, *rp;

  /* Obtain address(es) matching host/port. */

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
  hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
  hints.ai_flags = 0;
  hints.ai_protocol = 0; /* Any protocol */

  s = getaddrinfo(host, port, &hints, &result);
  if (s != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
    exit(EXIT_FAILURE);
  }

  /* getaddrinfo() returns a list of address structures.
  Try each address until we successfully connect(2).
  If socket(2) (or connect(2)) fails, we (close the socket
  and) try the next address. */

  for (rp = result; rp != NULL; rp = rp->ai_next) {
    sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    if (sfd == -1) continue;

    if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1) break; /* Success */

    close(sfd);
  }

  freeaddrinfo(result); /* No longer needed */

  if (rp == NULL) { /* No address succeeded */
    fprintf(stderr, "Could not connect\n");
    exit(EXIT_FAILURE);
  }

  /* Send remaining command-line arguments as separate
  datagrams, and read responses from server. */

  for (;;) {
    printf("> ");
    fgets(output, BUF_SIZE, stdin);
    len = strnlen(output, BUF_SIZE);

    if (len > BUF_SIZE) {
      fprintf(stderr, "Ignoring long message in argument\n");
      continue;
    }

    if (write(sfd, output, len) != len) {
      fprintf(stderr, "partial/failed write\n");
      exit(EXIT_FAILURE);
    }

    nread = read(sfd, buf, BUF_SIZE);
    if (nread == -1) {
      perror("read");
      exit(EXIT_FAILURE);
    }
    buf[nread - 1] = '\0';
    printf("Received %zd bytes: %s\n", nread, buf);
  }

 exit(EXIT_SUCCESS);
}

int main(void) {
  client("127.0.0.1", "3000");
  return 0;
}
