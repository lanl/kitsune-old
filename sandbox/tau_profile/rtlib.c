#include <stdio.h>

static const char * name = "some function";

void tau_prof_func_call(char * name) {
  fprintf(stderr, "Calling %s \n", name);
}


void tau_prof_func_ret(char * name) {
  fprintf(stderr, "Returned from %s \n", name);
}
