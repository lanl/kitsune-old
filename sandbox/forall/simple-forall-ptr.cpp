#include <iostream>
using namespace std;
#include <stdlib.h>

int main(int argc, char *argv[]) {
  int N = argc > 1 ? atoi(argv[1]) : 100; 
  if (N < 10) 
    N = 10;
  double *a = new double(N);

  int i = 0; 
  [[tapir::strategy("dac")]]
    forall(double* p = &(a[0]); p != &(a[N]); p++) {
    *p = i;
    i++;
  }

  [[tapir::strategy("seq")]]
  forall(int i = 0; i < 10; i++) {
    cout << "a[" << i << "] = " << a[i] <<  endl;
  }

  return 0;
}
