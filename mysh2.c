#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(int argc, char const *argv[]) {
  char prog[100];
  char *arg[10], *p;
  int i = 0;
  char *env_args[] = {"PATH=/bin", (char*)0};

  while (1) {
    fprintf(stdout, "$ ");
    if (fgets( prog, sizeof(prog), stdin) != NULL) {
      prog[strcspn(prog, "\n")] = 0;
      p = strtok(prog, " ");
      while (p != NULL) {
        arg[i++] = prog;
        p = strtok(NULL, " ");
      }
      arg[i+1] = "0";
    }
    if (strcmp(prog,"exit")==0) {
      exit(1);
    } else {
      if (fork()==0) {
        execle(arg[0], arg, env_args);
        perror("Error calling exec()! \n");
        exit(1);
      } else {
        wait(NULL);
      }
    }
  }
  return 0;
}
