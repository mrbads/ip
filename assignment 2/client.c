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

int main(int argc, char *argv[]) {
  char const *hostname = argv[1], *port = argv[2];
  struct addrinfo hints, *result, *p;
  int fd, status;
  char msg[3][256], *key, *value;

  // printf("%i\n", argc);
  // printf("%s\n", hostname);
  // printf("%s\n", port);

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
    for (size_t i = 0; i < argc; i++) {
      if (i < 2) {
      } else {
        if (strcmp(argv[i],"put") == 0) {
          printf("put\n");
          memset(msg, PUT, 1);
          key = argv[i+1];
          value = argv[i+2];
          strcpy(msg[1], key);
          strcpy(msg[2], value);
          if (write(fd, msg, sizeof(msg)) < 0) {
            fprintf(stderr, "write error\n");
          }
          i = i+2;
        } else if (strcmp(argv[i],"get") == 0) {
          printf("get\n");
          memset(msg, GET, 1);
          key = argv[i+1];
          strcpy(msg[1], key);
          if (write(fd, msg, sizeof(msg)) < 0) {
            fprintf(stderr, "write error\n");
          }
          i++;
        }
      }
    }


  }


  freeaddrinfo(result);
  return 0;
}