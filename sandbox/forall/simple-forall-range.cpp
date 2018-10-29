#include <iostream>
using namespace std;
#include <stdlib.h>

int main(int argc, char *argv[]) {
  const int N = argc > 1 ? atoi(argv[1]) : 100; 
  double a[N];

  //[[tapir::strategy("seq")]]
  forall(auto &element : a) {
    element = 0.0;
  }

  return 0;
}
