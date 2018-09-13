#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(int argc, char const *argv[]) {
  char input[100];
  char *prog, *path;

  while (1) {
    fprintf(stdout, "$ ");
    if (fgets( input, sizeof(input), stdin) != NULL) {
      input[strcspn(input, "\n")] = 0;
      prog = strtok(input, " ");
      path = strtok(NULL, " ");
    }
    if (strcmp(prog,"exit")==0) {
      exit(1);
    } else if (strcmp(prog,"cd")==0) {
      chdir(path);
    } else {
      if (fork()==0) {
        execlp(prog, prog, NULL);
        perror("Error calling exec()! \n");
        exit(1);
      } else {
        wait(NULL);
      }
    }
  }
  return 0;
}
