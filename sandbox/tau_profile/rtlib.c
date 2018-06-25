#include <stdio.h>


void tau_prof_func_call(const char * name) {
  fprintf(stderr, "Calling %s \n", name);
}


void tau_prof_func_ret(const char * name) {
  fprintf(stderr, "Returned from %s \n", name);
}
