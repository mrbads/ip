// Client

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

int GET = 103;
int PUT = 112;
int FOUND = 102;
int NOTFOUND = 110;

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

int main(int argc, char *argv[]) {
  char const *hostname = argv[1], *port = argv[2];
  struct addrinfo hints, *result, *p;
  int fd, status;
  char msg[3][256], *key, *value, ans[256], response_f[1], response_n[1];

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  status = getaddrinfo(hostname, port, &hints, &result);
  if (status != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
    exit(EXIT_FAILURE);
  }


  for (size_t i = 0; i < argc; i++) {
    if (i < 3) {
    } else {
      for (p = result; p != NULL; p = p->ai_next) {
        fd = socket(p->ai_family, p->ai_socktype, 0);
        if (fd == -1) {
          fprintf(stderr, "socket error\n");
          exit(EXIT_FAILURE);
        }
        if (connect(fd, p->ai_addr, p->ai_addrlen) == -1) {
        fprintf(stderr, "connect error\n");
        exit(EXIT_FAILURE);
        }        if (strcmp(argv[i],"put") == 0) {
          // printf("put\n");
          memset(msg, PUT, 1);
          key = argv[i+1];
          value = argv[i+2];
          strcpy(msg[1], key);
          strcpy(msg[2], value);
          if (writen(fd, msg, sizeof(msg)) < 0) {
            fprintf(stderr, "write error\n");
          }
          i = i+2;
          close(fd);
        } else if (strcmp(argv[i],"get") == 0) {
          // printf("get\n");
          memset(msg, GET, 1);
          key = argv[i+1];
          strcpy(msg[1], key);
          if (writen(fd, msg, sizeof(msg)) < 0) {
            fprintf(stderr, "write error\n");
          }
          if (read(fd, ans, sizeof(ans)) < 0) {
            fprintf(stderr, "read error\n");
            exit(EXIT_FAILURE);
          }
          memset(response_f, FOUND, 1);
          memset(response_n, NOTFOUND, 1);
          if ((memcmp(ans, response_f, 1)) == 0) {
            memmove(ans, ans+1, strlen(ans));
            printf("%s\n", ans);
          } else if ((memcmp(ans, response_n, 1)) == 0) {
            printf("\n");
          }

          i++;
          close(fd);
        }
      }
    }
  }

  freeaddrinfo(result);
  return 0;
}
