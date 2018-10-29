#include <iostream>
using namespace std;
#include <stdlib.h>

int main(int argc, char *argv[]) {
  int N = argc > 1 ? atoi(argv[1]) : 100; 
  if (N < 10) 
    N = 10;
  double *a = new double(N);

  [[tapir::strategy(STRATEGY)]]
  forall(int i = 0; i < N; i++) {
    a[i] = double(i);
  }

  [[tapir::strategy(STRATEGY)]]
  forall(int i = 0; i < 10; i++) {
    cout << "a[" << i << "] = " << a[i] <<  endl;
  }

  return 0;
}
