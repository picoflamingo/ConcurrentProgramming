//#define _GNU_SOURCE
#include <stdio.h>
#include <stdatomic.h>

int main () {
  int my_var = 0;
  int expected = 1;
  int _Atomic my_at_var = 0;

  __sync_bool_compare_and_swap (&my_var, 0, 1);
  atomic_compare_exchange_strong (&my_at_var, &expected, 0);
  
}
