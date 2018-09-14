#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(int argc, char const *argv[]) {
  int i = 0;
  char prog[1024];
  char *arg[10]={}, *p;

  while (1) {
    fprintf(stdout, "$ ");
    if (fgets(prog, sizeof(prog), stdin) != NULL) {
      i = 0;
      prog[strcspn(prog, "\n")] = 0;
      p = strtok(prog, " ");
      while (p != NULL) {
        arg[i++] = p;
        p = strtok(NULL, " ");
      }
      arg[9] = '\0';
    }

    if (strcmp(prog,"exit")==0) {
      exit(1);
    } else if (strcmp(prog, "cd")==0) {
      chdir(arg[1]);
    } else {
      if (fork()==0) {
        execvp(arg[0], arg);
        perror("Error calling exec()! \n");
        exit(1);
      } else {
        wait(NULL);
      }
    }
  }
  return 0;
}
