#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_ITER 100000

int             shared_var = 0;
pthread_mutex_t shared_var_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct tpar {
  int id;
  int cnt;
} TPAR;

void * task (void *_p)
{
  int v;
  TPAR  *p = (TPAR*)_p;
  while (1) {

    printf ("%02d: %d\n", p->id, shared_var);
    //pthread_mutex_lock (&shared_var_mutex);
    v = shared_var;
    shared_var = v + 1;
    //shared_var++;
    //pthread_mutex_unlock (&shared_var_mutex);
    p->cnt++;

    
  }
}



int main ()
{
  int        i;
  void       *r;
  TPAR       tp1, tp2,tp3;
  pthread_t  tid1, tid2, tid3;
  pthread_mutex_t mutex;

  tp1.id = 1;
  tp1.cnt = 0;
  if (pthread_create (&tid1, NULL, task, &tp1) < 0) exit (EXIT_FAILURE);

  tp2.id = 2;
  tp2.cnt = 0;

  if (pthread_create (&tid2, NULL, task, &tp2) < 0)  exit (EXIT_FAILURE);

  tp3.id = 3;
  tp3.cnt = 0;
  if (pthread_create (&tid3, NULL, task, &tp3) < 0)  exit (EXIT_FAILURE);
  
  i = 0;
  while (1) {
    i++;
    usleep (100);
    if (i >= MAX_ITER) 	break;
      
  }
  pthread_cancel (tid1);
  pthread_cancel (tid2);
  pthread_cancel (tid3);
  pthread_join (tid1, &r);
  pthread_join (tid2, &r);
   pthread_join (tid3, &r);
   
  printf ("Thread %d  run %d times\n", tp1.id, tp1.cnt);
  printf ("Thread %d  run %d times\n", tp2.id, tp2.cnt);
  printf ("Thread %d  run %d times\n", tp3.id, tp3.cnt);
  printf ("Shared Value : %d  (total threads : %d) \n",
	  shared_var, tp1.cnt+tp2.cnt+tp3.cnt);
  if (shared_var != tp1.cnt+tp2.cnt+tp3.cnt)
    printf ("\n*** FAIL ***\n");
  else
    printf ("\n*** SUCCESS ***\n");
  return 0;
}
