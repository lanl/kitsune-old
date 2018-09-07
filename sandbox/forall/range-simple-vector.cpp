#include <iostream>
#include <vector>
using namespace std;

const size_t SIZE = 1024*1024*1024;

int main(){
  vector<size_t> my_vect(SIZE);
  size_t value=0;
  
  [[tapir::strategy("dac")]]  
  forall(auto& i: my_vect){
    i = value;
    value++;
  }

  [[tapir::strategy("dac")]]
  forall(auto i: my_vect)
    cerr << i << endl;

  return 0;
}
