#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(int argc, char const *argv[]) {
  int x = 0, count = 0;
  char prog[100];
  char *arg[][10]={}, *p;
  int (*pfd)[2];
  char buf[100];

  while (1) {
    fprintf(stdout, "$ ");
    if (fgets(prog, sizeof(prog), stdin) != NULL) {
      p = prog;
      prog[strcspn(prog, "\n")] = 0;
      x = 0;
      arg[1][1] = ".";
      arg[2][0] = '\0';
      while ((p = strchr(p,'|')) != NULL) {
        count++;
        p++;
      }
      if (count != 0) {
        for (size_t i = 0; i < count; i++) {
          p = strtok(prog, "|");
          if (p != NULL) {
            p = strtok(p, " ");
            while (p != NULL) {
              arg[i][x++] = p;
              p = strtok(NULL, " ");
            }
            arg[i][9] = '\0';
          }
        }
      } else {
        p = strtok(prog, " ");
        if (p != NULL) {
          p = strtok(p, " ");
          while (p != NULL) {
            arg[0][x++] = p;
            p = strtok(NULL, " ");
          }
          arg[0][9] = '\0';
        }
      }
    }
    printf("%s\n", prog);
    printf("%s\n", arg[0][1]);

    if (strcmp(prog, "exit")==0) {
      exit(1);
    } else if (strcmp(prog, "cd")==0) {
      printf("%s\n", arg[0][1]);
      chdir(arg[0][1]);
      getcwd(buf, sizeof(buf));
      printf("%s\n", buf);
    } else {
      for (size_t i = 0; i < count-1; i++) {
        pipe(pfd[i]);
      }
      for (size_t i = 0; i < count; i++) {
        if (i == 0) {
          if (fork()==0) {
            printf("First fork\n");
            if (arg[2][0] != NULL) {
              dup2(*pfd[1], 1);
              close(*pfd[1]);
            }
            execvp(arg[1][0], arg[1]);
            perror("Error calling exec()! \n");
            exit(1);
          }
        } else if (i < count-1) {
          if (fork()==0) {
            dup2(*pfd[i-1], 0);
            dup2(*pfd[i+1], 1);
            close(*pfd[i-1]);
            close(*pfd[i+1]);
          }
        } else {
          if (fork()==0) {
            dup2(*pfd[i-1], 0);
            close(*pfd[i-1]);
          }
        }
      }
      for (size_t i = 0; i < count; i++) {
        close(*pfd[i]);
      }
      wait(NULL);
    }
    sleep(1);
  }
  return 0;
}
