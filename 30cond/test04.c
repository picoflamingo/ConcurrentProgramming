#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <pthread.h>

#define RUN_TIME       5
#define MAX_SIZE       64
#define MAX_THREADS    8
#define MAX_PRODUCERS  3


typedef struct t_par {
  int  id;
  int  flag;
} TPAR;

/* Shared buffer */
int shared_buf[MAX_SIZE];
int n = 0;
int n_transactions = 0;

int  max_buf = 0;
long acc_buf = 0;
int  prod_wait = 0;
int  cons_wait = 0;

pthread_mutex_t  mutex;

void *producer (void *arg)  {
  TPAR   *par = (TPAR*)arg;
  int id = par->id;

  while (!par->flag) {
    pthread_mutex_lock (&mutex); // Lock the resource
    while (n == MAX_SIZE - 1) {
      prod_wait++;
      pthread_mutex_unlock (&mutex);
      usleep (0);
      if (par->flag) return 0;
      pthread_mutex_lock (&mutex);
    }
    shared_buf[n] = rand () % 100;
    printf ("\033[32m Producer %d : %d (%d items)\033[m\n", id, shared_buf[n], n+1);
    //usleep (10000);
    n++;
    if (n > max_buf) max_buf = n;
    acc_buf += n;
    pthread_mutex_unlock (&mutex);
    usleep (100);
  }
  //printf ("** Producer %d terminated\n", id);
}

void *consumer (void *arg)  {
  TPAR   *par = (TPAR*)arg;
  int     id = par->id;

  while (!par->flag) { 
    pthread_mutex_lock (&mutex);
    while (n == 0) {
      cons_wait++;
      pthread_mutex_unlock (&mutex);
      usleep (0);
      if (par->flag) return 0;
      pthread_mutex_lock (&mutex);
    }
    n_transactions ++;
    n--;
    //usleep (10000);
    printf ("\033[31m Consumer %d : %d (%d items-%d)\033[m\n", id, shared_buf[n], n, n_transactions);    
    pthread_mutex_unlock (&mutex);
    usleep (100);
  }
  //printf ("** Consumer %d terminated\n", id);
}


int main () {
  pthread_t   tid[MAX_THREADS];
  TPAR        tpar[MAX_THREADS] = {0};
  int         i;

  srand (time(NULL));
  /* Initialise synchronisation objects*/
  pthread_mutex_init (&mutex, NULL);

  /* Launch threads */
  for (i = 0; i < MAX_THREADS; tpar[i++].id=i)
    pthread_create (&tid[i], NULL, (i > MAX_PRODUCERS - 1? consumer : producer), (void*)&tpar[i]);

  sleep (RUN_TIME);
  for (i = 0; i < MAX_THREADS; i++ ) tpar[i].flag= 1; 
  for (i = 0; i < MAX_THREADS; i++ ) pthread_join(tid[i], NULL);

  printf ("WAIT: Prod: %d || Cons: %d\n", prod_wait, cons_wait);  
  printf ("Buffer size: %d (%d-%ld)|| %d Producers  || %d Consumers\n",
	  MAX_SIZE, max_buf, acc_buf/n_transactions, MAX_PRODUCERS, MAX_THREADS-MAX_PRODUCERS);
  printf ("%d transactions processed\n", n_transactions);
  

  /* We will never reach this part*/
  return 0;
}
