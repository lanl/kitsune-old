#
#
kokkos_flags=-I/projects/kitsune/local/kokkos/include
opt_flags=-O0

klang=$(KITSUNE_HOME)/local/kitsune/bin/clang++
klang_flags=-std=c++11 -fopenmp -fkokkos -ftapir=cilk ${opt_flags} ${kokkos_flags}

clang=$(KITSUNE_HOME)/local/llvm/bin/clang++
clang_flags=-std=c++11 -fopenmp ${opt_flags} ${kokkos_flags}

sources=$(wildcard *.cpp)
cobjects=$(patsubst %.cpp, %_clang.o, $(sources))
kobjects=$(patsubst %.cpp, %_klang.o, $(sources))

clang_all:${cobjects}
	echo "done!"

klang_all:${kobjects}
	echo "done!"

%_clang.o: %.cpp 
	${clang} -c ${clang_flags} -o $@ $<

%_klang.o: %.cpp 
	${klang} -c ${klang_flags} -o $@ $<



