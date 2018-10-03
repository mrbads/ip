// multi-threaded server

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

pthread_mutex_t mutex, put_mutex;
pthread_mutexattr_t attr;

static int fd;

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

void *recv_requests(void *arg) {
  // iterative server

  int newsock;
  struct sockaddr_in client_addr;
  socklen_t addrlen;
  char msg[3][256], req_p[1], req_g[1], *answer, terug[256];

  printf("thread created\n");
  addrlen = sizeof(client_addr);

  while (1) {
    pthread_mutex_lock(&mutex);
    newsock = accept(fd, (struct sockaddr *) &client_addr, &addrlen);
    if (newsock < 0) {
      fprintf(stderr, "accept error\n");
      // exit(EXIT_FAILURE);
    }
    pthread_mutex_unlock(&mutex);
    printf("Connection from %s\n", inet_ntoa(client_addr.sin_addr));

    if (read(newsock, msg, sizeof(msg)) < 0) {
      fprintf(stderr, "read error\n");
      // exit(EXIT_FAILURE);
    }
    memset(req_p, PUT, 1);
    memset(req_g, GET, 1);
    printf("message: ");
    if (memcmp(msg[0], req_p, 1) == 0) {
      printf("put\nkey: %s\nvalue: %s\n", msg[1], msg[2]);
      pthread_mutex_lock(&put_mutex);
      put(msg[1], msg[2]);
      pthread_mutex_unlock(&put_mutex);
    } else if (memcmp(msg[0], req_g, 1) == 0) {
      printf("get\nkey: %s\n", msg[1]);
      answer = get(msg[1]);
      if (strcmp(answer, "NULL") != 0) {
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
  }
}

int main(int argc, char const *argv[]) {
  int option=1, port;
  struct sockaddr_in addr;
  socklen_t addrlen;
  pthread_t *thread;
  thread = (pthread_t *)malloc(4 * sizeof(pthread_t));

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
  pthread_mutexattr_init(&attr);
  if (pthread_mutex_init(&mutex, &attr) != 0) {
    fprintf(stderr, "mutex init failed\n");
  }
  if (pthread_mutex_init(&put_mutex, &attr) != 0) {
    fprintf(stderr, "put_mutex init failed\n");
  }

  for (size_t i = 0; i < 4; i++) {
    if ((pthread_create(&thread[i], NULL, &recv_requests, (void *)&fd)) != 0) {
      fprintf(stderr, "thread create error\n");
    }
    if ((pthread_detach(thread[i])) != 0) {
      fprintf(stderr, "thread detach error\n");
    }
  }

  while (1) {
    /* code */
  }

  return 0;
}
