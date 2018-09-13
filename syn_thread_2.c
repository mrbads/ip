#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "display.h"
#include <pthread.h>
#include <stdio.h>

pthread_mutex_t mutex;
pthread_cond_t cond_var;
int predicate = 0;

void* ab(void* arg) {
  pthread_mutex_lock(&mutex);
  display("ab");
  predicate = 1;
  pthread_cond_signal(&cond_var);
  pthread_mutex_unlock(&mutex);
}

void* cd(void* arg) {
  pthread_mutex_lock(&mutex);
  while (predicate==0) {
    pthread_cond_wait(&cond_var, &mutex);
  }
  display("cd\n");
  predicate = 0;
  pthread_mutex_unlock(&mutex);
}

int main(int argc, char const *argv[]) {
  int i;
  pthread_t t1, t2;

  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&cond_var, NULL);

  for (size_t i = 0; i < 10; i++) {
    pthread_create(&t1, NULL, &ab, NULL);
    pthread_create(&t2, NULL, &cd, NULL);
  }

  pthread_join(t1, NULL);
  pthread_join(t2, NULL);
  pthread_mutex_destroy(&mutex);
  return 0;
}
