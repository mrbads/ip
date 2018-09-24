#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "display.h"
#include <sys/ipc.h>
#include <sys/sem.h>


int main()
{
  int i;
  struct sembuf up = {0, 1, 0};
  struct sembuf down = {0, -1, 0};
  int my_sem;

  my_sem = semget(IPC_PRIVATE, 1, 0600);

  if (fork()) {
    for (size_t i = 0; i < 10; i++) {
      semop(my_sem, &down, 1);
      display("Hello world\n");
      semop(my_sem, &up, 1);
    }
    wait(NULL);
  } else {
    // sleep(2);
    semop(my_sem, &up, 1);
    for (size_t i = 0; i < 10; i++) {
      semop(my_sem, &down, 1);
      display("Bonjour monde\n");
      semop(my_sem, &up, 1);
    }
  }

  semctl(my_sem, 0, IPC_RMID);
  return 0;
}
