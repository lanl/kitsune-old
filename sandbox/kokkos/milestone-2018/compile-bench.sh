#!/bin/bash

for opt_flag in O0 O1 O2 O3
do
    echo "************** Optimization flag: ${opt_flag}"
    for loop_count in 1 2 3 4 5 10 15 30 60 120
    do
	filename="forall-kokkos-${loop_count}.cpp"
	exefile=`basename $filename .cpp`
	echo "----- $filename"
	echo "clang ${opt_flag}:"
	echo /projects/kitsune/local/llvm/bin/clang++ -std=c++11 -${opt_flag} -fopenmp -I/projects/kitsune/local/kokkos/include $filename -o $exefile.clang.${opt_flag} -L/projects/kitsune/local/kokkos/lib -L/projects/kitsune/local/lib -lkokkos -ldl	
	/usr/bin/time -p /projects/kitsune/local/llvm/bin/clang++ -std=c++11 -${opt_flag} -fopenmp -I/projects/kitsune/local/kokkos/include $filename -o $exefile.clang.${opt_flag} -L/projects/kitsune/local/kokkos/lib -L/projects/kitsune/local/lib -lkokkos -ldl
	echo "kitsune ${opt_flag}:"
	echo /projects/kitsune/local/kitsune/bin/kokkos-clang++ -std=c++11 -${opt_flag} -fopenmp -fkokkos -ftapir=cilk -I/projects/kitsune/local/kokkos/include $filename -o $exefile.kitsune.${opt_flag} -L/projects/kitsune/local/kokkos/lib -L/projects/kitsune/local/lib -lkokkos -ldl 		
	/usr/bin/time -p /projects/kitsune/local/kitsune/bin/kokkos-clang++ -std=c++11 -${opt_flag} -fopenmp -fkokkos -ftapir=cilk -I/projects/kitsune/local/kokkos/include $filename -o $exefile.kitsune.${opt_flag} -L/projects/kitsune/local/kokkos/lib -L/projects/kitsune/local/lib -lkokkos -ldl
	echo "-----"
    done
done
