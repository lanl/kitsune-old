#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
const size_t n = 10000000;

int main(){
  int *a = (int*)malloc(n * sizeof(int));  
  
  #pragma omp parallel for
  for(int i=0; i<n; i++){
    a[i] = 0; 
  }

  #pragma omp parallel for
  for(int i=0; i<n; i++){
    a[i] = a[i] + i; 
  }

  uint64_t sum=0;
  for(int i=0; i<n; i++){
    sum += a[i];
  }

  printf("sum = %lu\n", sum);
}
