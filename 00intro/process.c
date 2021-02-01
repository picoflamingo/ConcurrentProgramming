#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>  // Needed for wait
#include <sys/wait.h>


int global_var = 1;

int main ()
{
  pid_t child;

  // Create a new process
  if ((child = fork ()) < 0) {
    perror ("fork:");
    exit (EXIT_FAILURE);
  }
  if (child == 0) // Child process
    {
      global_var ++;
      printf ("CHILD: GLobal variable : %d\n", global_var);
      return 0; // The child process finish
    }
  else // This is the father
    {
      int status;
      wait (&status);
      sleep (1);
      printf ("FATHER: GLobal variable : %d\n", global_var);
    }

  return 0;
}
