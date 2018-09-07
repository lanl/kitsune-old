tapirclang=../../../build/llvm/bin/clang
clang=clang
cflags=-O2 -g
tapirflags=-mllvm -fast-openmp -ftapir=cilk -fopenmp -fPIC ${cflags}
ompflags=-fopenmp ${cflags}

types=gomp omp serial tapir  
sources=fib nqueens merge daxpy par_for
targets=$(foreach f,${sources},${f}_tapir $(foreach t, ${types}, ${f}_${t}))

all: ${targets}

timeing.txt: ${targets}
	$(foreach t, ${targets}, time ./${t})	

%.s: %.c
	${tapirclang} -S -O2 -fopenmp -ftapir=openmp $<

%.ll: %.c
	${tapirclang} -S -O2 -fopenmp -emit-llvm $<
	
%_lowered.ll: %.c
	${tapirclang} -S -O2 -ftapir=openmp -fopenmp -emit-llvm $<

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
