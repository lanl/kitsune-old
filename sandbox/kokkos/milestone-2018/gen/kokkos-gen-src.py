#!/usr/bin/python3
import sys

header="""
#include <cstdio>
#include <iostream>
#include <Kokkos_Core.hpp>
using namespace std;
const size_t SIZE = 1073741824;

int main (int argc, char* argv[]) {
  Kokkos::initialize (argc, argv);
  typedef Kokkos::View<double*[2]> view_type;
  view_type a ("A", SIZE);

  for(size_t i = 0; i < SIZE; ++i){
    a(i, 0) = 0.0;
    a(i, 1) = 0.0;
  }
""";

pforloop="""

  Kokkos::parallel_for (SIZE, KOKKOS_LAMBDA (const int i) {
    a(i, 0) += i * 100.0;
    a(i, 1) += i * 10000.0;
  });

""";

trailer="""
  for(size_t i = 0; i < 1024; ++i){
    double ai0 = a(i, 0);
    double ai1 = a(i, 1);
    cout << "a(" << i << ", 0) = " << ai0 << endl;
    cout << "a(" << i << ", 1) = " << ai1 << endl;
  }

  Kokkos::finalize ();
  return 0;
 }
""";

filename="forall-kokkos-"+sys.argv[1]+".cpp"
print(filename)
file = open(filename,"w")

file.write(header)

for count in range(1,int(sys.argv[1])+1):
    file.write(pforloop)
    
file.write(trailer)
file.close()
