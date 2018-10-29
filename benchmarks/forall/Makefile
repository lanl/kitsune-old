tapirclang=$(KITSUNE_HOME)/bin/clang++
clang=clang++
cflags=-O3 -g -std=c++14 
tapirflags=${cflags} -ftapir=cilk -fPIC
qthreadsflags=${cflags} -ftapir=qthreads -fPIC
ompflags=-fopenmp ${cflags}
ldflags= -L$(KITSUNE_HOME)/../lib
types=serial tapir qthreads 
sources=daxpy
targets=$(foreach f,${sources}, $(foreach t, ${types}, ${f}_${t}))

all: ${targets}

timing.txt: ${targets}
	$(foreach t, ${targets}, time ./${t})	

%.s: %.cpp
	${tapirclang} -S -O3 -fopenmp -ftapir=openmp $< -o $@

%.ll: %.cpp
	${tapirclang} -S -O3 -fopenmp -emit-llvm $< -o $@

%_lowered.ll: %.cpp
	${tapirclang} ${tapirflags} -S -emit-llvm $< -o $@

%_gomp: %.cpp
	g++ ${ompflags} -o $@ $< ${ldflags}

%_omp: %.cpp
	${clang} ${ompflags} -o $@ $< ${ldflags}

%_serial: %.cpp
	${clang} ${cflags} -Dforall="for" -o $@ $< ${ldflags}

%_tapir: %.cpp
	${tapirclang} ${tapirflags} -o $@ $< ${ldflags}

%_qthreads: %.cpp
	${tapirclang} ${cflags} ${qthreadsflags} -o $@ $< ${ldflags}


clean: 
	rm -f ${targets}
