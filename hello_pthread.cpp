#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

int thread_count;

void *Hello (void *rank);

int main(int argc, char* argv[]) {
  long thread;
  pthread_t* thread_handles;

  thread_count = strtol(argv[1], NULL, 10);
  thread_handles = (pthread_t*) malloc(thread_count * sizeof(pthread_t));

  for (thread = 0; thread < thread_count; thread++) {
    // thread memory location, pthread attribute, start routine, argument to the routine
    pthread_create(&thread_handles[thread], NULL, Hello, (void *)thread);
  }
  printf(" I am the main thread \n");
  
  for (thread = 0; thread < thread_count; thread++) {
    pthread_join(thread_handles[thread], NULL);
  }

  free(thread_handles);
  return 0;
}

void *Hello(void *rank){
  long my_rank = (long) rank; // use long in case of 64bit system
  printf("Hello from pthread %ld of %d \n", my_rank, thread_count);
  return NULL;
}
