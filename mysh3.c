#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(int argc, char const *argv[]) {
  int i = 0, pfd[2];
  char prog[1024];
  char *arg1[10]={}, *arg2[10]={}, *p, *q;

  while (1) {
    fprintf(stdout, "$ ");
    if (fgets(prog, sizeof(prog), stdin) != NULL) {
      i = 0;
      prog[strcspn(prog, "\n")] = 0;
      p = strtok(prog, "|");
      q = strtok(NULL, "\n");
      if (p != NULL) {
        p = strtok(p, " ");
        while (p != NULL) {
          arg1[i++] = p;
          p = strtok(NULL, " ");
        }
        arg1[9] = '\0';
      }
      if (q != NULL) {
        q = strtok(q, " ");
        i = 0;
        while (q != NULL) {
          arg2[i++] = q;
          q = strtok(NULL, " ");
        }
        arg2[9] = '\0';
      }
    }

    if (strcmp(prog, "exit")==0) {
      exit(1);
    } else if (strcmp(prog, "cd")==0) {
      chdir(arg1[1]);
    } else {
      pipe(pfd);
      if (fork()==0) {
        dup2(pfd[1], 1);
        close(pfd[0]);
        close(pfd[1]);
        execvp(arg1[0], arg1);
        perror("Error calling exec()! \n");
        exit(1);
      } else {
        if (fork()==0) {
          dup2(pfd[0], 0);
          close(pfd[0]);
          close(pfd[1]);
          execvp(arg2[0], arg2);
          perror("Error calling exec()! \n");
          exit(1);
        }
      }
      close(pfd[0]);
      close(pfd[1]);
      wait(NULL);
    }
  }
  return 0;
}
