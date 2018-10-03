// talk program

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int fd_exit, BACKLOG = 5;
char *kill_msg = "killquit";

void close_prog(int sig) {
  printf("end the convo\n");
  write(fd_exit, kill_msg, sizeof(kill_msg -1));

  exit(1);
}

ssize_t writen(int fd, const void *vptr, size_t n) {
  size_t nleft;
  ssize_t nwritten;
  const char *ptr;

  ptr = vptr;
  nleft = n;
  while (nleft > 0) {
    if ((nwritten = write(fd, ptr, nleft)) <= 0) {
      if (errno == EINTR) {
        nwritten = 0;
      } else {
        return -1;
      }
    }
    nleft -= nwritten;
    ptr += nwritten;
  }
  return n;
}

void conversation(int fd) {
  fd_set chatfds;
  char msg[256], recv_msg[256];
  int nb, recv, n;

  fd_exit = fd;
  signal(SIGINT, close_prog);
  while (1) {
    FD_ZERO(&chatfds);
    FD_SET(fd, &chatfds);
    FD_SET(0, &chatfds);
    nb = select(20, &chatfds, NULL, NULL, NULL);
    if (nb <= 0) {    }
    if (FD_ISSET(fd, &chatfds)) {
      bzero(recv_msg, 256);
      recv = read(fd, recv_msg, sizeof(recv_msg));
      if (strcmp(recv_msg, kill_msg) == 0) {
        exit(1);
      }
      printf("%s", recv_msg);
      FD_CLR(fd, &chatfds);
    }
    if (FD_ISSET(0, &chatfds)) {
      fgets(msg, sizeof(msg), stdin);
      if (writen(fd, msg, sizeof(msg)) < 0) {
        fprintf(stderr, "write error\n");
      }
      FD_CLR(0, &chatfds);
    }
  }
}

void server(const char *port) {
  int fd, option=1, newsock, port_s;
  struct sockaddr_in addr, client_addr;
  socklen_t addrlen;
  char recv_msg[256];

  printf("server mode\n");

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

    conversation(newsock);
  }
}

void client(const char *hostname, const char *port) {
  struct addrinfo hints, *result, *p;
  int fd, status, nb, recv;
  char msg[256], recv_msg[256];
  fd_set clientfds;

  printf("client mode\n");

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
  conversation(fd);
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
