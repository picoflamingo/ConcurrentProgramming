#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <pthread.h>

#define RUN_TIME       5
#define MAX_SIZE       64
#define MAX_THREADS    8
#define MAX_PRODUCERS  3

/* Shared buffer */
int  shared_buf[MAX_SIZE];
int  n = 0;

/* Statistics */
int  n_transactions = 0;
int  max_buf = 0;
long acc_buf = 0;
int  prod_wait = 0;
int  cons_wait = 0;

pthread_mutex_t  mutex;
pthread_cond_t   full;
pthread_cond_t   empty;

void *producer (void *arg)  {
  int id = (int)arg;

  while (1) {
    pthread_mutex_lock (&mutex);
    while (n == MAX_SIZE - 1) { prod_wait++; pthread_cond_wait (&full, &mutex);}

    shared_buf[n] = rand () % 100;
    printf ("\033[32m Producer %d : %d (%d items)\033[m\n", id, shared_buf[n], n+1);
    //usleep (10000);
    n++;
    if (n > max_buf) max_buf = n;
    acc_buf += n;
    pthread_cond_signal (&empty);
    //pthread_cond_broadcast (&empty);
    pthread_mutex_unlock (&mutex);
    usleep (100);
  }
}

void *consumer (void *arg)  {
  int id = (int)arg;

  while (1) { 
    pthread_mutex_lock (&mutex);
    while (n == 0) {cons_wait++; pthread_cond_wait (&empty, &mutex);}


    n_transactions++;
    n--;
    printf ("\033[31m Consumer %d : %d (%d items-%d)\033[m\n", id, shared_buf[n], n, n_transactions);
    //usleep (10000);
    pthread_cond_signal (&full);
    //pthread_cond_broadcast (&full);

    pthread_mutex_unlock (&mutex);
    usleep (100);
  }
}


int main () {
  pthread_t   tid[MAX_THREADS];
  int         i;

  srand (time(NULL));
  /* Initialise synchronisation objects*/
  pthread_mutex_init (&mutex, NULL);
  pthread_cond_init  (&empty, NULL);
  pthread_cond_init  (&full, NULL);

  /* Launch threads */
  for (i = 0; i < MAX_THREADS; i++)
    pthread_create (&tid[i], NULL, (i > MAX_PRODUCERS - 1? consumer : producer), (void*)i);

  sleep (RUN_TIME);
  printf ("WAIT: Prod: %d || Cons: %d\n", prod_wait, cons_wait);
  printf ("Buffer size: %d (%d-%ld) || %d Producers  || %d Consumers\n",
	  MAX_SIZE, max_buf, acc_buf / n_transactions, MAX_PRODUCERS, MAX_THREADS-MAX_PRODUCERS);

  printf ("%d transactions processed\n", n_transactions);
  
  /* We will never reach this part*/
  return 0;
}
