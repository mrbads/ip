#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "display.h"
#include <pthread.h>

int main() {
  int i;
  pthread_t t1, t2;

  if (fork()) {
    for (size_t i = 0; i < 10; i++) {
      display("Hello world\n");
    }
    wait(NULL);
  } else {
    for (size_t i = 0; i < 10; i++) {
      display("Bonjour monde\n");
    }
  }
  return 0;
}
