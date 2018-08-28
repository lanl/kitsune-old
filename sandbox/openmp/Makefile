buildir=/dev/shm/build
all: test

test: test.c
	${buildir}/bin/clang -g -L${buildir}/lib/clang/5.0.0/lib/linux -fopenmp -ftapir=qthreads -O2 test.c -lclang_rt.kitsune-powerpc64le -lcuda -lstdc++ -o $@

test.ll: test.c
	${buildir}/bin/clang -fopenmp -S -emit-llvm test.c
	
