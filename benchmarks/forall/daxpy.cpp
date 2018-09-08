
#include <iostream>
#include <vector>
#include <chrono>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>


using namespace std; 

int main(int argc, char** argv){
  int n = argc > 1 ? atoi(argv[1]) : 10000000; 
  int m = argc > 1 ? atoi(argv[1]) : 100;   

  vector<double> a(n), b(n), c(n) ;
  double alpha=0.3; 

  [[tapir::strategy("dac")]]
  forall(int i=0; i<n; i++){
    a[i] = (double)i; 
    b[i] = 3.0; 
    c[i] = 0; 
  }

  for(int j=0; j<m; j++){
    [[tapir::strategy("dac")]]      
    forall(int i=0; i<n; i++){
      c[i] += a[i] + b[i] * alpha; 
    }
  }
  printf("c[10] = %f\n", c[10]);
  printf("done\n");  
  return 0;
}
