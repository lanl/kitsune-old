tapirclang=../../../build/llvm/bin/clang++
clang=clang++
cflags=-O2 -g -std=c++14 
tapirflags=-mllvm -fast-openmp -ftapir=cilk -fPIC ${cflags}
ompflags=-fopenmp ${cflags}

types=serial tapir  
sources=daxpy
targets=$(foreach f,${sources},${f}_tapir $(foreach t, ${types}, ${f}_${t}))

all: ${targets}

timeing.txt: ${targets}
	$(foreach t, ${targets}, time ./${t})	

%.s: %.cpp
	${tapirclang} -S -O2 -fopenmp -ftapir=openmp $<

%.ll: %.cpp
	${tapirclang} -S -O2 -fopenmp -emit-llvm $<
	
%_lowered.ll: %.cpp
	${tapirclang} ${tapirflags} -S -emit-llvm $<

%_gomp: %.cpp
	g++ ${ompflags} -o $@ $<

%_omp: %.cpp
	${clang} ${ompflags} -o $@ $<

%_serial: %.cpp
	${tapirclang} ${cflags} -o $@ $<

%_tapir: %.cpp
	${tapirclang} ${tapirflags} -o $@ $<

clean: 
	rm -f ${targets}
