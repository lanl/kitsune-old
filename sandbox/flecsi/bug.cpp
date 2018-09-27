#include <iostream>
using namespace std;

#define SIZE 20
//const unsigned int SIZE = 20;

int main(int argc, char** argv){

  int a[SIZE], b[SIZE];

  for(int i = 0; i < SIZE; i++) {
    a[i] = i;
    b[i] = a[i] * 2;
  }

  cout << "a: ";
  for(int i = 0; i < SIZE; i++) {
    cout << a[i] << " ";
  }
  cout << endl << endl;;

  cout << "b: ";
  for(int i = 0; i < SIZE; i++) {
    cout << b[i] << " ";
  }
  cout << endl << endl;;

  int j = 0;
  [[tapir::strategy("dac")]]
  forall(int &i : b) {
    i += a[j];
    j++;
  }

  cout << "b: ";
  for(int i = 0; i < SIZE; i++) {
    cout << b[i] << " ";
  }
  cout << endl;

  return 0;
}
