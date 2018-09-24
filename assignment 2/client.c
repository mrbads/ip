// Client

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>

int GET = 103;
int PUT = 112;
int FOUND = 102;
int NOTFOUND = 110;

int connect(int sockfd, const struct sockaddr *serv_addr, socklen_t addrlen);

int main(int argc, char const *argv[]) {
  char const *hostname = argv[1], *port = argv[2];

  printf("%s\n", hostname);
  printf("%s\n", port);
  return 0;
}
