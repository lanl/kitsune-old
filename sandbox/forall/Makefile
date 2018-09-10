#
#
clang=$(KITSUNE_HOME)/local/kitsune/bin/clang++
clang_flags=-std=c++14 -fno-exceptions 
tapir_flags=-DSTRATEGY=\"seq\"
opt=$(KITSUNE_HOME)/local/kitsune/bin/opt
opt_flags=-O2 
ld_flags=-L/$(KITSUNE_HOME)/local/lib 
cilk_flags=-ftapir=cilk 
omp_flags=-fopenmp


%: %.cpp 
	${clang} ${clang_flags} -o $@ ${opt_flags} $< 

%_serial: %.cpp
	${clang} ${clang_flags} -o $@ ${opt_flags} $< 

%_cilk: %.cpp
	${clang} ${clang_flags} ${tapir_flags} -o $@ ${cilk_flags} ${opt_flags} ${ld_flags} $<

%_omp: %.cpp
	${clang} ${clang_flags} ${omp_flags} ${tapir_flags} -o $@ ${cilk_flags} ${opt_flags} ${ld_flags} $<

%.ll: %.cpp
	${clang} ${clang_flags} -S -emit-llvm ${opt_flags} -o $@ $<

# this is a hack that will only work for the main-based routnes in the
# sandbox codes for now...
%.dot: %.ll
	(mkdir .tmp-dots-$@; cd .tmp-dots-$@; ${opt} -dot-cfg ../$<)
	mv .tmp-dots-$@/cfg.main.dot $@
	rm -rf .tmp-dots-$@

%.pdf: %.dot
	dot -Tpdf -o $@ $<

