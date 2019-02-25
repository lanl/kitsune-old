#
#
SHELL := bash 

ifndef KITSUNE_INSTALL_DIR
	$(error enviornment variable KITSUNE_INSTALL_DIR is undefined.)
endif

clang=$(KITSUNE_INSTALL_DIR)/local/kitsune/bin/clang++
clang_flags=-std=c++14 -fno-exceptions

tapir_flags=-DTAPIR_STRATEGY=\"seq\"
opt=$(KITSUNE_INSTALL_DIR)/local/kitsune/bin/opt
opt_flags=-O2 
ld_flags=-L/$(KITSUNE_INSTALL_DIR)/local/lib 
cilk_flags=-ftapir=cilk 
omp_flags=-fopenmp


%_seq: %.cpp
	@(test -d build || (mkdir build))
	@(test -d build/$@.d || (mkdir build/$@.d))
	${clang} ${clang_flags} -S -emit-llvm ${opt_flags} -o build/$@.d/$@.ll $<
	@(cd build/$@.d ; ${opt} -dot-cfg $@.ll)
	@(cd build/$@.d ; (for fn in *.dot ; do dot -Tpdf -o $$fn.pdf $$fn; done))
	${clang} ${clang_flags} ${opt_flags} -o build/$@ $<


%_omp: %.cpp
	@(test -d build || (mkdir build))
	@(test -d build/$@.d || (mkdir build/$@.d))
	${clang} ${clang_flags} ${omp_flags} ${tapir_flags} -S -emit-llvm ${opt_flags} -o build/$@.d/$@.ll $<
	@(cd build/$@.d ; ${opt} -dot-cfg $@.ll)
	@(cd build/$@.d ; (for fn in *.dot ; do dot -Tpdf -o $$fn.pdf $$fn; done))
	${clang} ${clang_flags} ${omp_flags} ${tapir_flags} ${opt_flags} -o build/$@ ${ld_flags} $<


%_cilk: %.cpp
	@(test -d build || (mkdir build))
	@(test -d build/$@.d || (mkdir build/$@.d))
	${clang} ${clang_flags} ${tapir_flags} -S -emit-llvm ${opt_flags} -o build/$@.d/$@.ll $<
	@(cd build/$@.d ; ${opt} -dot-cfg $@.ll)
	@(cd build/$@.d ; (for fn in *.dot ; do dot -Tpdf -o $$fn.pdf $$fn; done))
	${clang} ${clang_flags} ${tapir_flags} ${opt_flags} -o build/$@ $< ${ld_flags}

# 
clean:
	@rm -rf build
