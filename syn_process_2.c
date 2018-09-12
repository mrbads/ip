#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "display.h"

int main(int argc, char const *argv[]) {
  int i;

  if (fork()) {
    for (size_t i = 0; i < 10; i++) {
      display("ab");
    }
    wait(NULL);
  } else {
    for (size_t i = 0; i < 10; i++) {
      display("cd\n");
    }
  }
  return 0;
}
