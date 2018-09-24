#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "display.h"
#include <pthread.h>
#include <stdio.h>

pthread_mutex_t mutex;

void* Print(void* arg) {
  pthread_mutex_lock(&mutex);
  display(arg);
  pthread_mutex_unlock(&mutex);
}

int main() {
  int i;
  pthread_t t1, t2;
  pthread_mutexattr_t attr;

  pthread_mutexattr_init(&attr);
  if (pthread_mutex_init(&mutex, &attr) != 0){
    fprintf(stderr, "mutex init failed\n");
    return 1;
  }

  for (size_t i = 0; i < 10; i++) {
    pthread_create(&t1, NULL, &Print, "Hello world\n");
    pthread_create(&t2, NULL, &Print, "Bonjour monde\n");
  }

  pthread_join(t1, NULL);
  pthread_join(t2, NULL);
  pthread_mutex_destroy(&mutex);
  return 0;
}
