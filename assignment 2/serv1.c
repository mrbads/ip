// Iterative server

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int GET = 103;
int PUT = 112;
int FOUND = 102;
int NOTFOUND = 110;
int BACKLOG = 5;


int main(int argc, char const *argv[]) {
  int fd, newsock, res;
  struct sockaddr_in addr, client_addr;
  socklen_t addrlen;
  char buf[100];
  socklen_t fromlen = sizeof(client_addr);

  fd = socket(AF_INET, SOCK_STREAM, 0);
  addrlen = sizeof(struct sockaddr_in);
  memset(&addr, 0, addrlen);
  addr.sin_family = AF_INET;
  addr.sin_port = htons(1235);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(fd, (struct sockaddr *) &addr, addrlen) == -1) {
    fprintf(stderr, "bind error");
    exit(EXIT_FAILURE);
  }

  printf("host: %s\n", inet_ntoa(addr.sin_addr));

  res = listen(fd, BACKLOG);
  while (1) {
    newsock = accept(fd, (struct sockaddr *) &client_addr, &addrlen);

    if (newsock < 0) {
      /* code */
    } else {
      printf("Connection from %s\n", inet_ntoa(client_addr.sin_addr));
    }
    close(newsock);
  }

  close(fd);
  return 0;
}
