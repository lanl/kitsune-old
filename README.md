OpenMP -> Tapir 
================================

This is a research project that compiles a subset of OpenMP to the Tapir LLVM
IR extensions.

To compile, use both `-fopenmp` and `-ftapir=openmp`. 

Building
========

    git clone https://github.com/lanl/openmpir-clang
    git clone https://github.com/wsmoses/parallel-ir
    rmdir parallel-ir/tools/clang
    ln -s $PWD/openmpir-clang parallel-ir/tools/clang
    mkdir build
    cd build
    cmake ../parallel-ir
    make -j `grep -c '^processor' /proc/cpuinfo` 
    cd ../openmpir-clang/benchmarks
    make -j `grep -c '^processor' /proc/cpuinfo` 
