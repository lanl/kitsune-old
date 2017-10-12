
tapirclang=../../build/bin/clang
clang=clang
cflags=-O3 -g
tapirflags=-mllvm -fast-openmp -ftapir=openmp -fopenmp ${cflags}
ompflags=-fopenmp ${serialflags}

types=gomp omp serial tapir  
sources=fib nqueens merge 
targets=$(foreach f,${sources},${f}_tapir $(foreach t, ${types}, ${f}_${t}))

all: ${targets}

timeing.txt: ${targets}
	$(foreach t, ${targets}, time ./${t})	

%.ll: %.c
	${clang} -S -fopenmp -emit-llvm $<
	
%_gomp: %.c
	gcc ${ompflags} -o $@ $<

%_omp: %.c
	${clang} ${ompflags} -o $@ $<

%_serial: %.c
	${tapirclang} ${serialflags} -o $@ $<

%_tapir: %.c
	${tapirclang} ${tapirflags} -o $@ $<

clean: 
	rm -f ${targets}
