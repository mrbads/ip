#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(int argc, char const *argv[]) {
  char input[100];
  char *prog, *path;
  char cwd[256];
  int fd[2];
  char buf[64];

  while (1) {
    if (pipe(fd)<0) exit(1);
    getcwd(cwd, sizeof(cwd));
    fprintf(stdout, "$ ");
    if (fgets( input, sizeof(input), stdin) != NULL) {
      input[strcspn(input, "\n")] = 0;
      prog = strtok(input, " ");
      path = strtok(NULL, " ");
    }
    if (strcmp(prog,"exit")==0) {
      exit(1);
    } else if (strcmp(prog,"cd")==0) {
      fprintf(stdout, "%s\n", cwd);
      // fork and make the child change directory
      if (fork()==0) {
        close(fd[0]);
        chdir(path);
        getcwd(cwd, sizeof(cwd));
        write(fd[1], cwd, 14);
        exit(1);
      } else {
        wait(NULL);
        close(fd[1]);
        if (read(fd[0], buf, 64) > 0) {
          fprintf(stdout, "%s\n", buf);
        }
        fprintf(stdout, "%s\n", cwd);
      }
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
