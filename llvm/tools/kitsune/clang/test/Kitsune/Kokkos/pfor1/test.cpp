// RUN: (cd %S; make clean && make && ./test.host)

/*
 * ###########################################################################
 * Copyright (c) 2016, Los Alamos National Security, LLC All rights
 * reserved. Copyright 2016. Los Alamos National Security, LLC. This
 * software was produced under U.S. Government contract DE-AC52-06NA25396
 * for Los Alamos National Laboratory (LANL), which is operated by Los
 * Alamos National Security, LLC for the U.S. Department of Energy. The
 * U.S. Government has rights to use, reproduce, and distribute this
 * software.  NEITHER THE GOVERNMENT NOR LOS ALAMOS NATIONAL SECURITY,
 * LLC MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR ASSUMES ANY LIABILITY
 * FOR THE USE OF THIS SOFTWARE.  If software is modified to produce
 * derivative works, such modified software should be clearly marked, so
 * as not to confuse it with the version available from LANL.
 *  
 * Additionally, redistribution and use in source and binary forms, with
 * or without modification, are permitted provided that the following
 * conditions are met: 1.       Redistributions of source code must
 * retain the above copyright notice, this list of conditions and the
 * following disclaimer. 2.      Redistributions in binary form must
 * reproduce the above copyright notice, this list of conditions and the
 * following disclaimer in the documentation and/or other materials
 * provided with the distribution. 3.      Neither the name of Los Alamos
 * National Security, LLC, Los Alamos National Laboratory, LANL, the U.S.
 * Government, nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior
 * written permission.
  
 * THIS SOFTWARE IS PROVIDED BY LOS ALAMOS NATIONAL SECURITY, LLC AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL LOS
 * ALAMOS NATIONAL SECURITY, LLC OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE US
 * ########################################################################### 
 * 
 * Notes
 *
 * ##### 
 */

#include <Kokkos_Core.hpp>
#include <cstdio>
#include <iostream>
#include <cassert>

using namespace std;

const size_t SIZE = 26843545;
const size_t SIZE2 = 512;

int main (int argc, char* argv[]) {
  Kokkos::initialize (argc, argv);

  typedef Kokkos::View<double*[2]> view_type;

  view_type a ("A", SIZE);

  for(size_t i = 0; i < SIZE; ++i){
    a(i, 0) = 0.0;
    a(i, 1) = 0.0;
  }

  Kokkos::parallel_for (SIZE, KOKKOS_LAMBDA (const int i) {
    a(i, 0) += i * 100.0;
    a(i, 1) += i * 10000.0;
  });

  for(size_t i = 0; i < SIZE; ++i){
    assert(a(i, 0) == i * 100.0);
    assert(a(i, 1) == i * 10000.0);
  }

  typedef Kokkos::View<double**[2]> view_type2;

  view_type2 b ("B", SIZE2, SIZE2);

  for(size_t i = 0; i < SIZE2; ++i){
    for(size_t j = 0; j < SIZE2; ++j){
      b(i, j, 0) = 0.0;
      b(i, j, 1) = 0.0;
    }
  }

  Kokkos::parallel_for (SIZE2, KOKKOS_LAMBDA (const int i) {
    Kokkos::parallel_for (SIZE2, KOKKOS_LAMBDA (const int j) {
      b(i, j, 0) += i * 100000.0 + j * 10000.0 + 0;
      b(i, j, 1) += i * 100000.0 + j * 10000.0 + 1;
    });
  });

  for(size_t i = 0; i < SIZE2; ++i){
    for(size_t j = 0; j < SIZE2; ++j){
      assert(b(i, j, 0) == i * 100000.0 + j * 10000.0 + 0);
      assert(b(i, j, 1) == i * 100000.0 + j * 10000.0 + 1);
    }
  }

  Kokkos::finalize ();

  return 0;
}
