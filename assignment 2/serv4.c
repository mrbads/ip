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
#include <pthread.h>

#include "keyvalue.h"

int GET = 103;
int PUT = 112;
int FOUND = 102;
int NOTFOUND = 110;
int BACKLOG = 5;

pthread_mutex_t mutex;

void *recv_requests(void *arg) {
  // iterative server

  int newsock, *fd = (int *)arg;
  struct sockaddr_in client_addr;
  socklen_t addrlen;
  char msg[3][256], req_p[1], req_g[1], *answer, terug[256];

  printf("thread created\n");
  addrlen = sizeof(client_addr);

  while (1) {
    pthread_mutex_lock(&mutex);
    newsock = accept(*fd, (struct sockaddr *) &client_addr, &addrlen);
    if (newsock < 0) {
      fprintf(stderr, "accept error\n");
      exit(EXIT_FAILURE);
    }
    pthread_mutex_unlock(&mutex);
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
      put(msg[1], msg[2]);
    } else if (memcmp(msg[0], req_g, 1) == 0) {
      printf("get\nkey: %s\n", msg[1]);
      answer = get(msg[1]);
      if (strcmp(answer, "NULL") != 0) {
        printf("%s\n", answer);
        memset(terug, FOUND, 1);
        strcat(terug, answer);
        printf("%s\n", terug);
      } else {
        memset(terug, NOTFOUND, 1);
        printf("%s\n", terug);
      }
      bzero(terug, 256);
    }
    close(newsock);
  }
}

int main(int argc, char const *argv[]) {
  int fd, option=1, port;
  struct sockaddr_in addr;
  socklen_t addrlen;
  pthread_t *thread;
  thread = (pthread_t *)malloc(2 * sizeof(pthread_t));

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
  pthread_mutex_init(&mutex, NULL);

  for (size_t i = 0; i < 2; i++) {
    if ((pthread_create(&thread[i], NULL, recv_requests, (void *)&fd)) != 0) {
      fprintf(stderr, "thread create error\n");
    }
    if ((pthread_detach(thread[i])) != 0) {
      fprintf(stderr, "thread detach error\n");
    }
  }

  return 0;
}
