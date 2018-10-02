// talk program

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int BACKLOG = 5;

void server(const char *port) {
  int fd, option=1, newsock, port_s;
  struct sockaddr_in addr, client_addr;
  socklen_t addrlen;

  port_s = strtol(port, NULL, 10);
  fd = socket(AF_INET, SOCK_STREAM, 0);
  addrlen = sizeof(struct sockaddr_in);
  memset(&addr, 0, addrlen);
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port_s);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(fd, (struct sockaddr *) &addr, addrlen) == -1) {
    fprintf(stderr, "bind error\n");
    exit(EXIT_FAILURE);
  }
  if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *) &option, sizeof(option)) < 0) {
    fprintf(stderr, "setsockopt failed\n");
    exit(EXIT_FAILURE);
  }
  listen(fd, BACKLOG);

  while (1) {
    newsock = accept(fd, (struct sockaddr *) &client_addr, &addrlen);
    if (newsock < 0) {
      fprintf(stderr, "accept error\n");
      exit(EXIT_FAILURE);
    }
    printf("Connection from %s\n", inet_ntoa(client_addr.sin_addr));
  }
}

void client(const char *hostname, const char *port) {
  struct addrinfo hints, *result, *p;
  int fd, status;

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
}

int main(int argc, char const *argv[]) {
  char const *hostname, *port;

  if (argc == 2) {
    // server mode
    port = argv[1];
    server(port);
  } else if (argc == 3) {
    // client mode
    hostname = argv[1];
    port = argv[2];
    client(hostname, port);
  }
  return 0;
}
