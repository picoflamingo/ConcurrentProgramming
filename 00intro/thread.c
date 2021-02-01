#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>

int global_var = 1;

void *func (void *p) {
  global_var ++;
  printf ("THREAD: Global variable: %d\n", global_var);
}

int main ()
{
  pthread_t tid;
  void      *r;

  if (pthread_create (&tid, NULL, func, NULL) < 0) exit (EXIT_FAILURE);
  pthread_join (tid, &r);
  printf ("MAIN: Global variable: %d\n", global_var);
  return 0;
}
