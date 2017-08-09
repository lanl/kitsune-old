#include <stdio.h>
#include <stdlib.h>

int fib(int n){
  if(n < 2) return n;
  int x = 0; 
  #pragma omp task shared(x) 
  x += fib(n-1);
  #pragma omp task shared(x) 
  x += fib(n-2); 
  #pragma omp taskwait
  return x;
}

int main(int argc, char** argv){
  int x, n = argc > 1 ? atoi(argv[1]) : 40; 

  #pragma omp parallel shared(x)
  #pragma omp single nowait
  x = fib(n); 

  printf("fib(%d) = %d\n", n, x);
}
