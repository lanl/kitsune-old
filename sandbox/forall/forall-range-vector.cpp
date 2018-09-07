
#include <iostream>
#include <vector>
#include <chrono>
#include <math.h>

#define SIZE 100

using namespace std; 

int main(){

  vector<int> my_vect(SIZE);

  int value=0;
  forall(auto& i: my_vect){
    i = value;
    ++value;
  }

  [[tapir::strategy("dac")]]
  forall(auto i: my_vect)
    cout << i << endl;

  return 0;
}
