KOKKOS_PATH = /home/nickm/kokkos
SRC = $(wildcard *.cpp)

default: build
	echo "Start Build"

#CXX = /home/nickm/kitsune/build/llvm/bin/clang++ -std=c++14 -isystem /projects/opt/centos7/gcc/5.2.0/include/c++/5.2.0 -I/projects/opt/centos7/gcc/5.2.0/include/c++/5.2.0/x86_64-unknown-linux-gnu
CXX = /home/nickm/kitsune/build/llvm/bin/kokkos-clang++ -std=c++14 -fkokkos
CXX_NO_EXCEPT = /home/nickm/kitsune/build/llvm/bin/kokkos-clang++ -std=c++14 -fkokkos -fno-exceptions

LINK = ${CXX}
#LINKFLAGS = -L/home/nickm/kokkos-clang/build/runtime -L/usr/local/cuda/lib64 -L/projects/opt/centos7/gcc/5.2.0/packages/gcc-5.2.0/lib64 
LINKFLAGS = 
EXE = $(SRC:.cpp=.host)
KOKKOS_DEVICES = "Pthread"
KOKKOS_ARCH = "SNB"
DEPFLAGS = -M

OBJ = $(SRC:.cpp=.o)
#LIB = -lideas_runtime -lcuda -lcudart
LIB = 

include $(KOKKOS_PATH)/Makefile.kokkos

build: $(EXE)

$(EXE): $(OBJ) $(KOKKOS_LINK_DEPENDS)
	$(LINK) $(KOKKOS_LDFLAGS) $(LINKFLAGS) $(EXTRA_PATH) $(OBJ) $(KOKKOS_LIBS) $(LIB) -o $(EXE)

clean: kokkos-clean 
	rm -f *.o *.cuda *.host

# Compilation rules

test.o: test.cpp
	$(CXX_NO_EXCEPT) $(KOKKOS_CPPFLAGS) $(KOKKOS_CXXFLAGS) $(CXXFLAGS) $(EXTRA_INC) -c $<

%.o:%.cpp $(KOKKOS_CPP_DEPENDS)
	$(CXX) $(KOKKOS_CPPFLAGS) $(KOKKOS_CXXFLAGS) $(CXXFLAGS) $(EXTRA_INC) -c $<
