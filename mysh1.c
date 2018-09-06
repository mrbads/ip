#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(int argc, char const *argv[]) {
  char const *name = argv[1];
  char prog[100], var[100];
  char *cwd;
  char *buf;

  while (1) {
    scanf("%s", prog);
    if (strcmp(prog,"exit")==0) {
      exit(1);
    } else if (strcmp(prog,"cd")==0) {
      // fork and make the child change directory
      if (fork()==0) {
        cwd = getcwd(buf, 100);
        printf("%s\n", cwd);
      } else {
        wait(NULL);
        printf("$ ");
      }
    } else {
      if (fork()==0) {
        execlp(prog, prog, NULL);
        perror("Error calling exec()! \n");
        exit(1);
      } else {
        wait(NULL);
        printf("$ ");
      }
    }
  }
  return 0;
}
