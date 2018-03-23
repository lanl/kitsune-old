KOKKOS_PATH = /home/nickm/kokkos
SRC = $(wildcard *.cpp)

default: build
	echo "Start Build"

CXX = ../../../build/llvm/bin/kokkos-clang++ -std=c++14 -fkokkos -O2
CXX_NO_EXCEPT = ../../../build/llvm/bin/kokkos-clang++ -std=c++14 -fkokkos -fno-exceptions -O2 -Wno-macro-redefined

LINK = ${CXX}
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
