#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#include <sys/syscall.h>
#include <linux/futex.h>
#include <sys/time.h>


#define MAX_ITER 10000

#define MUTEX_WAITING 2
#define MUTEX_BUSY    1
#define MUTEX_FREE    0

int             shared_var = 0;
pthread_mutex_t shared_var_mutex = PTHREAD_MUTEX_INITIALIZER;

#define futex_wait(addr, val) futex (addr, FUTEX_WAIT, val, NULL, NULL, 0)
#define futex_wake(addr, val) futex (addr, FUTEX_WAKE, val, NULL, NULL, 0)

typedef struct tpar {
  int id;
  int cnt;
  int flag;
} TPAR;

typedef struct mmux_t {
  int  v;
} MY_MUTEX;

MY_MUTEX my_mutex_var = {MUTEX_FREE};

static int
futex(int *uaddr, int futex_op, int val,
      const struct timespec *timeout, int *uaddr2, int val3)
{
  return syscall(SYS_futex, uaddr, futex_op, val,
		 timeout, uaddr, val3);
}


#if 0
int futex_wait (int *addr, int val) {
  return futex (addr, FUTEX_WAIT, val, NULL, NULL, 0);
}
int futex_wake (int *addr, int val) {
  return futex (addr, FUTEX_WAKE, val, NULL, NULL, 0);
}
#endif

int my_mutex_lock (MY_MUTEX *mmux) {
  int v, r;

  // 1. Try to get the mutex supposing it is free
  if ((v = __sync_val_compare_and_swap (&mmux->v, MUTEX_FREE, MUTEX_BUSY)) != 0)
    {
      // 3. The mutex is locked so we have to sleep -> state chanes to WAITING
      do
	{
	  // 4. If the state was already waiting we go to sleep
	  //    If the state was BUSY, then we set it to waiting
	  //    In both cases we go to sleep and the state is WAITING
	  if (v == MUTEX_WAITING ||
	      __sync_val_compare_and_swap
	      (&mmux->v, MUTEX_BUSY, MUTEX_WAITING) != 0)
	    {
	      futex_wait (&mmux->v, MUTEX_WAITING);	      
	    }
	  // 5. When ever current thread wakes up a new thread we try to reacquire the mutex
	  //    If the mutex is free (no other thread got it in between) we go to WAITING
	  //    and get the lock because we do not know if there are more threads waiting
	  //    Otherwise we wait again
	} while ((v = __sync_val_compare_and_swap
		  (&mmux->v, MUTEX_FREE, MUTEX_WAITING)) != 0);

    }
  // 2. In case the mutex was free... we just go through
  /*
  while (1) {
    if (__sync_valbool_compare_and_swap (&mmux->v, MUTEX_FREE, MUTEX_BUSY)) break;
    r = futex_wait (&mmux->v, 0);
    if (r == -1 && errno != EAGAIN) {
      perror ("wait:");
      exit (EXIT_FAILURE);
    }
  }
  //while (!(__sync_bool_compare_and_swap (&mmux->v, MUTEX_FREE, MUTEX_BUSY)));
  */
}

int my_mutex_unlock (MY_MUTEX *mmux) {

#if 0
  // WORKS
  mmux->v = MUTEX_FREE;
  futex_wake (&mmux->v, 1);
#else
  // BUSY = WAITING - 1 -> There is something waiting... wake uo
  // FREE = BUSY - 1 -> The thread in the lock is leaving... nothing to do
  //if ((__sync_sub_and_fetch (&mmux->v, 1)) == MUTEX_BUSY) {
  if ((__sync_fetch_and_sub (&mmux->v, 1)) != MUTEX_BUSY) {
    mmux->v = MUTEX_FREE;
    //printf ("UNLOCK WAIT\n");
    futex_wake (&mmux->v, 1);
  }
  //else printf ("UNLOCK SHORTCUT\n");
    //printf ("%ld: Lock released (%d)\n", pthread_self(), mmux->v);
  
#endif
}

void * task (void *_p)
{
  int v;
  TPAR  *p = (TPAR*)_p;

 while (p->flag == 0) {
    printf ("%02d: %d\n", p->id, shared_var);
    my_mutex_lock (&my_mutex_var);
    v = shared_var;
    shared_var = v + 1;
    //shared_var++;
    usleep (100);
    my_mutex_unlock (&my_mutex_var);
    
    p->cnt++;

    
  }
   printf ("%02d: Task finished\n", p->id);
}



int main ()
{
  int        i;
  void       *r;
  TPAR       tp1, tp2, tp3;
  pthread_t  tid1, tid2, tid3;
  pthread_mutex_t mutex;
  
  tp1.id = 1;
  tp1.flag = tp1.cnt = 0;
  if (pthread_create (&tid1, NULL, task, &tp1) < 0) exit (EXIT_FAILURE);

 
  tp2.id = 2;
  tp2.flag = tp2.cnt = 0;
  if (pthread_create (&tid2, NULL, task, &tp2) < 0)  exit (EXIT_FAILURE);

  tp3.id = 3;
  tp3.flag = tp3.cnt = 0;
  if (pthread_create (&tid3, NULL, task, &tp3) < 0)  exit (EXIT_FAILURE);


  i = 0;
  while (1) {
    i++;
    usleep (100);
    if (i >= MAX_ITER) 	break;
      
  }
   printf ("******************************************\n");
  tp1.flag = 1;
  tp2.flag = 1;
  tp3.flag = 1;

  pthread_join (tid1, &r);
  pthread_join (tid2, &r);
  pthread_join (tid3, &r);
   
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
