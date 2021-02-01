#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/ipc.h>
#include <sys/shm.h>


#include <sys/types.h>  // Needed for wait
#include <sys/wait.h>


int main ()
{
  pid_t  child;
  key_t  key;
  int    shared_id;
  int    *global_var;

  // Create a unique key for the shared memory block
  key = ftok (".", 'd');
  //printf ("Key: %d\n", key);
  
  if ((shared_id= shmget (key, sizeof (int), IPC_CREAT | 0666)) < 0) {
    perror ("shmget:");
    exit (EXIT_FAILURE);
  }
  
  global_var  = (int*) shmat (shared_id, NULL, 0);
  //printf ("Shared id: %d Memory mapped at: %p\n", shared_id, global_var);
  /*
  if (((int)global_var) < 0)
    {
      perror ("shmat:");
      exit (EXIT_FAILURE);
    }
  */
  *global_var = 11;
  
  // Create a new process
  if ((child = fork ()) < 0) {
    perror ("fork:");
    exit (EXIT_FAILURE);
  }
  if (child == 0) // Child process
    {
      //shared_id= shmget (key, sizeof (int), 0666);
      global_var  = (int*) shmat (shared_id, NULL, 0);
      *global_var = *global_var + 1;
      printf ("CHILD: GLobal variable : %d\n", *global_var);
      shmdt ((void*)global_var);
      return 0; // The child process finish
    }
  else // This is the father
    {
      int status;
      wait (&status);
      sleep (1);
      printf ("FATHER: GLobal variable : %d\n", *global_var);
      shmdt ((void*)global_var);
    }

  return 0;
}
