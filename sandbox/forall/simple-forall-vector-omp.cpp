#include <iostream>
#include <vector>
using namespace std;
#include <stdlib.h>

int main(int argc, char *argv[]) {
  int N = argc > 1 ? atoi(argv[1]) : 100; 
  vector<double> a(N);
  if (N < 10) 
    N = 10;

  int i = 0; 
  #pragma parallel for 
  forall(vector<double>::iterator it = a.begin(); it != a.end(); ++it) {
    *it = i;
    i++;
  }

  for(int i = 0; i < 10; i++) {
    cout << "a[" << i << "] = " << a[i] <<  endl;
  }

  return 0;
}
