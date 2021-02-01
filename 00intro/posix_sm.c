#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */

#include <sys/types.h>  // Needed for wait
#include <sys/wait.h>

#define SM_FILENAME "/sm"
#define SM_SIZE sizeof(int)

int main ()
{
  pid_t  child;
  key_t  key;
  int    shared_id;
  int    *global_var;

  if ((shared_id = shm_open (SM_FILENAME, O_CREAT | O_RDWR, 0666)) < 0) {
    perror ("shm_open1:");
    exit (EXIT_FAILURE);
  }
  ftruncate (shared_id, SM_SIZE);
  global_var = (int*) mmap (NULL, SM_SIZE, 0666, MAP_SHARED,
			    shared_id, 0);
  if (global_var == MAP_FAILED) {
    perror ("mmap:");
    exit (EXIT_FAILURE);
  }

  *global_var = 11;
  printf ("Creating Process...\n");
  // Create a new process
  if ((child = fork ()) < 0) {
    perror ("fork:");
    exit (EXIT_FAILURE);
  }
  if (child == 0) // Child process
    {
      global_var = (int*) mmap (NULL, SM_SIZE, 0666, MAP_SHARED,
			    shared_id, 0);

      *global_var = *global_var + 1;
      printf ("CHILD: GLobal variable : %d\n", *global_var);

      close (shared_id);
      munmap (global_var, SM_SIZE);
      shm_unlink (SM_FILENAME);
      return 0; // The child process finish
    }
  else // This is the father
    {
      int status;
      wait (&status);
      sleep (1);
      printf ("FATHER: GLobal variable : %d\n", *global_var);
      
      close (shared_id);
      munmap (global_var, SM_SIZE);
      shm_unlink (SM_FILENAME);
    }

  return 0;
}
