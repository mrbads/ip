#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

void run(char *prog, int rd, int wr) {
  char *p, *arg[10]={};
  int i = 0;

  p = strtok(prog, " ");
  if (p != NULL) {
    // printf("%s\n", p);
    while (p != NULL) {
      arg[i++] = p;
      p = strtok(NULL, " ");
    }
  }
  if (fork() == 0) {
    if (rd) {
      dup2(rd, 0);
    }
    if (wr) {
      dup2(wr, 1);
    }
    execvp(arg[0], arg);
  } else {
    if (rd) {
      close(rd);
    }
    if (wr) {
      close(wr);
    }
  }
}

int main(int argc, char const *argv[]) {
  int x, count=0, (*pfd)[2];
  char prog[1024];
  char *arg[10]={}, *p, *q;

  if (fgets(prog, sizeof(prog), stdin) != NULL) {
    // printf("%s\n", prog);
    prog[strcspn(prog, "\n")] = 0;
    p = prog;
  }
  while ((p = strchr(p, '|')) != NULL) {
    count++;
    p++;
  }
  // printf("%i\n", count);

  for (size_t i = 0; i < count+1; i++) {
    if (i == 0) {
      p = strtok(prog, "|");
      // printf("%s\n", p);
      arg[i] = p;
    } else {
      p = strtok(NULL, "|");
      // printf("%s\n", p);
      arg[i] = p;
    }
  }

  if (strcmp(prog, "exit") == 0) {
    exit(1);
  } else if (strcmp(prog, "cd") == 0) {
    printf("change dir\n");
  } else {
    if ((pfd = malloc(sizeof(*pfd) * count)) == NULL) {
      perror("malloc");
      exit(EXIT_FAILURE);
    }

    for (int i = 0; i < count; i++) {
      if (pipe(pfd[i]) < 0) {
        perror("pipe");
        exit(EXIT_FAILURE);
      }
    }
    for (size_t i = 0; i < count+1; i++) {
      if (i == 0) {
        run(arg[i], 0, pfd[i][1]);
      } else if (i < count) {
        run(arg[i], pfd[i-1][0], pfd[i][1]);
      } else {
        run(arg[i], pfd[i-1][0], 0);
      }
    }
  }

  return 0;
}
