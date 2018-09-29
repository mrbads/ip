// one-process-per-client server

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <search.h>

#include "keyvalue.h"

int GET = 103;
int PUT = 112;
int FOUND = 102;
int NOTFOUND = 110;
int BACKLOG = 5;

void sig_chld() {
  while (waitpid(0, NULL, WNOHANG) > 0) {
    signal(SIGCHLD, sig_chld);
  }
}

int main(int argc, char const *argv[]) {
  int fd, newsock, res, option=1, port;
  struct sockaddr_in addr, client_addr;
  socklen_t addrlen;
  char msg[3][256], *p, *answer, terug[256];
  socklen_t fromlen = sizeof(client_addr);
  pid_t pid;
  struct hsearch_data *hashtable;

  port = strtol(argv[1], NULL, 10);
  fd = socket(AF_INET, SOCK_STREAM, 0);
  addrlen = sizeof(struct sockaddr_in);
  memset(&addr, 0, addrlen);
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(fd, (struct sockaddr *) &addr, addrlen) == -1) {
    fprintf(stderr, "bind error");
    exit(EXIT_FAILURE);
  }

  if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*) &option, sizeof(option)) < 0) {
    fprintf(stderr, "setsockopt failed\n");
    exit(EXIT_FAILURE);
  }

  if (hcreate_r(64, hashtable) == 0) {
    fprintf(stderr, "hcreate error\n");
    exit(EXIT_FAILURE);
  }

  printf("host: %s\n", inet_ntoa(addr.sin_addr));

  res = listen(fd, BACKLOG);
  signal(SIGCHLD, sig_chld);
  while (1) {
    newsock = accept(fd, (struct sockaddr *) &client_addr, &addrlen);

    if (newsock < 0) {
      fprintf(stderr, "accept error\n");
      exit(EXIT_FAILURE);
    } else {
      pid = fork();
      if (pid == 0) {
        printf("Connection from %s\n", inet_ntoa(client_addr.sin_addr));
        printf("message: ");
        if (read(newsock, msg, sizeof(msg)) < 0) {
          fprintf(stderr, "read error\n");
          exit(EXIT_FAILURE);
        }
        if (strcmp(msg[0],"p") == 0) {
          printf("put\nkey: %s\nvalue: %s\n", msg[1], msg[2]);
          put(msg[1], msg[2]);
        } else if (strcmp(msg[0], "g") == 0) {
          printf("get\nkey: %s\n", msg[1]);
          answer = get(msg[1]);
          if ((strcmp(answer, "NULL")) != 0) {
            printf("%s\n", answer);
            memset(terug, FOUND, 1);
            strcat(terug, answer);
            printf("%s\n", terug);
            if (write(newsock, terug, sizeof(terug)) < 0) {
              fprintf(stderr, "write error\n");
            }
          } else {
            memset(terug, NOTFOUND, 1);
            printf("%s\n", terug);
            if (write(newsock, terug, sizeof(terug)) < 0) {
              fprintf(stderr, "write error\n");
            }
          }
          memset(terug, 0, sizeof(terug));
        }
      } else {
        close(newsock);
      }
    }
  }

  close(fd);
  return 0;
}
