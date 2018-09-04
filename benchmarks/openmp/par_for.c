#include <omp.h>
#include <stdio.h>
int main() {
        
  #pragma omp parallel for
  for(int i=0; i<10; i++){
    printf("Hello from iteration %d\n", i);
  }
  return 0;
}


