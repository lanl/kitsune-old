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

#define LOOP_COUNT 64
#define LOOP_NEST 1

using namespace std;

size_t uniform(size_t max){
  return max * rand()/RAND_MAX;
}

int main (int argc, char* argv[]) {
  Kokkos::initialize (argc, argv);

  srand(time(0));

#ifdef LOOP_NEST
  const size_t SIZE = 16384;
  typedef Kokkos::View<double**[2]> view_type;
  view_type a ("A", SIZE, SIZE);
#else
  const size_t SIZE = 536870912;
  typedef Kokkos::View<double*[2]> view_type;
  view_type a ("A", SIZE);
#endif

  #include "pfor_loop.h"

  #if LOOP_COUNT > 1
  #include "pfor_loop.h"
  #endif

  #if LOOP_COUNT > 2
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #endif

  #if LOOP_COUNT > 4
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #endif

  #if LOOP_COUNT > 8
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #endif

  #if LOOP_COUNT > 16
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #endif

  #if LOOP_COUNT > 32
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #endif

  #if LOOP_COUNT > 64
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #include "pfor_loop.h"
  #endif

  #ifdef LOOP_NEST
    size_t i = uniform(SIZE);
    size_t j = uniform(SIZE);
    cout << a(i, j, 0) << endl;
    cout << a(i, j, 1) << endl;
  #else
    size_t i = uniform(SIZE);
    cout << a(i, 0) << endl;
    cout << a(i, 1) << endl;
  #endif

  Kokkos::finalize ();

  return 0;
}
