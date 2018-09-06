#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(int argc, char const *argv[]) {
  char const *name = argv[1];
  char input[100];

  while (1) {
    scanf("%s", input);
    if (strcmp(input,"exit")==0) {
      exit(1);
    } else if (strcmp(input,"cd")==0) {
      printf("probably cd\n");
      // fork and make the child change directory
    } else {
      if (fork()==0) {
        execlp(input, input, NULL);
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
