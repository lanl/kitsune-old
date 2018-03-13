Kitsune 
================================

This is a research project investigating frontends and backends to the Tapir
LLVM IR extensions.

Building with OpenMP -> Tapir Clang
===================================
    mkdir build
    cd build
    cmake ..
    make

Building with Flecsi+Kokkos -> Tapir Clang
==========================================
    mkdir build
    cd build
    cmake .. -DKITSUNE_CLANG=on
    make 

Running
=======
    ./build/llvm/bin/clang <source_file> <source_flags> -ftapir=<backend rts>

Currently supported backend runtimes are `cilk` and `openmp`, though the `cilk`
backend is more mature.  

Issues
=======
- Parallel for doesn't currently work with OpenMP backend

