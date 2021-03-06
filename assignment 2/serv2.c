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
#include <wait.h>
#include <errno.h>
#include <sys/sem.h>
#include <sys/ipc.h>

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

ssize_t writen(int fd, const char *vptr, size_t n) {
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

int main(int argc, char const *argv[]) {
  int fd, newsock, res, option=1, port, my_sem;
  struct sockaddr_in addr, client_addr;
  socklen_t addrlen;
  char msg[3][256], *p, *answer, terug[256], req_p[1], req_g[1];
  socklen_t fromlen = sizeof(client_addr);
  pid_t pid;
  struct sembuf up = {0, 1, 0};
  struct sembuf down = {0, -1, 0};

  my_sem = semget(IPC_PRIVATE, 1, 0600);
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

  create(64);

  printf("host: %s\n", inet_ntoa(addr.sin_addr));

  listen(fd, BACKLOG);
  signal(SIGCHLD, sig_chld);
  while (1) {
    newsock = accept(fd, (struct sockaddr *) &client_addr, &addrlen);
    if (newsock < 0) {
      fprintf(stderr, "accept error\n");
      exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid == 0) {
      printf("Connection from %s\n", inet_ntoa(client_addr.sin_addr));

      if (read(newsock, msg, sizeof(msg)) < 0) {
        fprintf(stderr, "read error\n");
        exit(EXIT_FAILURE);
      }
      memset(req_p, PUT, 1);
      memset(req_g, GET, 1);
      printf("message: ");
      if (memcmp(msg[0], req_p, 1) == 0) {
        printf("put\nkey: %s\nvalue: %s\n", msg[1], msg[2]);
        semop(my_sem, &up, 1);
        put(msg[1], msg[2]);
        semop(my_sem, &down, 1);
      } else if (memcmp(msg[0], req_g, 1) == 0) {
        printf("get\nkey: %s\n", msg[1]);
        answer = get(msg[1]);
        if ((strcmp(answer, "NULL")) != 0) {
          printf("%s\n", answer);
          memset(terug, FOUND, 1);
          strcat(terug, answer);
          printf("%s\n", terug);
          if (writen(newsock, terug, sizeof(terug)) < 0) {
            fprintf(stderr, "write error\n");
          }
        } else {
          memset(terug, NOTFOUND, 1);
          if (writen(newsock, terug, sizeof(terug)) < 0) {
            fprintf(stderr, "write error\n");
          }
        }
        bzero(terug, 256);
      }
      close(newsock);
    } else {
      // parent
    }
  }

  semctl(my_sem, 0, IPC_RMID);
  close(fd);
  return 0;
}
