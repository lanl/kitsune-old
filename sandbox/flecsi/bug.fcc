#include <iostream>
using namespace std;

const unsigned int SIZE = 100;

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

  forall(int &i : b) {
    i += a[i];
  }

  cout << "b: ";
  for(int i = 0; i < SIZE; i++) {
    cout << b[i] << " ";
  }
  cout << endl;

  return 0;
}
