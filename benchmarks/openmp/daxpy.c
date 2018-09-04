#include <stdlib.h>
#include <stdio.h>

int main(int argc, char** argv){
  int n = argc > 1 ? atoi(argv[1]) : 100000000; 
  double *a=malloc(n*sizeof(double));
  double *b=malloc(n*sizeof(double)); 
  volatile double *c=malloc(n*sizeof(double)); 
  double alpha=0.3; 
  #pragma omp parallel for
  for(int i=0; i<n; i++){
    a[i] = (double)i; 
    b[i] = 3.0; 
  }
  #pragma omp parallel for
  for(int i=0; i<n; i++){
    c[i] = a[i] + b[i] * alpha; 
  }
  printf("done\n");  
}
