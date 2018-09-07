
#include <iostream>
#include <vector>
#include <chrono>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>


using namespace std; 

int main(int argc, char** argv){
  int n = argc > 1 ? atoi(argv[1]) : 1000000; 
  int m = argc > 1 ? atoi(argv[1]) : 10; 

  vector<double> a(n), b(n), c(n) ;
  double alpha=0.3; 

  for(int i=0; i<n; i++){
    a[i] = (double)i; 
    b[i] = 3.0; 
    c[i] = 0; 
  }

  for(int j=0; j<m; j++){
    forall(int i=0; i<n; i++){
      c[i] += a[i] + b[i] * alpha; 
    }
  }
  printf("done\n");  
  return 0;
}
