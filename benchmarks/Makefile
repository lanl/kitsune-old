
tapirclang=../../build/bin/clang
clang=clang
cflags=-O2 -g
tapirflags=-mllvm -fast-openmp -ftapir=openmp -fopenmp ${cflags}
ompflags=-fopenmp ${cflags}

types=gomp omp serial tapir  
sources=fib nqueens merge daxpy
targets=$(foreach f,${sources},${f}_tapir $(foreach t, ${types}, ${f}_${t}))

all: ${targets}

timeing.txt: ${targets}
	$(foreach t, ${targets}, time ./${t})	

%.s: %.c
	${tapirclang} -S -O2 -fopenmp -ftapir=openmp $<

%.ll: %.c
	${tapirclang} -S -O2 -fopenmp -emit-llvm $<
	
%_gomp: %.c
	gcc ${ompflags} -o $@ $<

%_omp: %.c
	${clang} ${ompflags} -o $@ $<

%_serial: %.c
	${tapirclang} ${cflags} -o $@ $<

%_tapir: %.c
	${tapirclang} ${tapirflags} -o $@ $<

clean: 
	rm -f ${targets}
