// Client

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int GET = 103;
int PUT = 112;
int FOUND = 102;
int NOTFOUND = 110;

int connect(int sockfd, const struct sockaddr *serv_addr, socklen_t addrlen);

int main(int argc, char *argv[]) {
  char const *hostname = argv[1], *port = argv[2];
  struct addrinfo hints, *result, *p;
  int fd, status;
  char msg[256];

  printf("%i\n", argc);
  printf("%s\n", hostname);
  printf("%s\n", port);
  if (strcmp(argv[4],"put") == 0) {
    msg = PUT + argv[5] + argv[6];
  } else if (strcmp(argv[4],"get") == 0) {
    msg = GET;
  }
  printf("%s\n", msg);

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  status = getaddrinfo(hostname, port, &hints, &result);
  if (status != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
    exit(EXIT_FAILURE);
  }

  for (p = result; p != NULL; p = p->ai_next) {
    fd = socket(p->ai_family, p->ai_socktype, 0);
    if (fd == -1) {
      fprintf(stderr, "socket error\n");
      exit(EXIT_FAILURE);
    }
    if (connect(fd, p->ai_addr, p->ai_addrlen) == -1) {
      fprintf(stderr, "connect error\n");
      exit(EXIT_FAILURE);
    }

  }


  freeaddrinfo(result);
  return 0;
}
