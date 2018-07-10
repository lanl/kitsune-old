# Adding TAU profiling calls via LLVM pass

## Building

This pass (in `Instrument.cpp`) can be built as part of the greater
Kitsune project, part of an LLVM build or independently, linked against
the LLVM libraries on your system.

### As part of Kitsune

Simply follow the Kitsune build instructions. You can find the shared
library at `<kitsune-build-dir>/llvm/lib/Tau_Profiling.so`.

### As part of a standard LLVM build

Copy this directory into `<llvm-root>/lib/Transform` and add the
following line to `<llvm-root>/lib/Transform/CMakeLists.txt`:

``` cmake
add_subdirectory(Tau)
```

### <span class="todo TODO">TODO</span> Test this

  - make sure the resulting library is in the expected `build/lib/`
    directory.

### Independently

Starting at `llvm/lib/Transform/Tau` as the working directory:

``` bash
mkdir -p build
cd build
cmake .. -DTAU_PROF_STANDALONE=1
cmake --build .
```

## Usage

The plugin accepts three optional command line arguments, that permit
the user to specify:

  - `-tau-start-func`  
    The function to call *before* each instrumented function call-site.
    By default this is `Tau_start`
  - `-tau-stop-func`  
    The function to call *after* each instrumented function call-site.
    By default this is `Tau_stop`
  - `-tau-input-file`  
    A file containing the names of functions to instrument. This has no
    default, but failing to specify such a file will result in no
    instrumentation.

They can be set using `clang`, `clang++`, or `opt` with LLVM bitcode
files. Only usage with Clang frontends is detailed here.

To use the plugin with the default start and stop functions, you must
know the path to the TAU shared library. To use alternative functions,
you'll need the path to the appropriate libraries for them.

At the moment, there are three source files and a sample file containing
function names in the `sandbox/tau_profile` directory that can be used
for simple tests.

  - `rtlib.c` defines two functions that could be used as alternatives
    to `Tau_start` and `Tau_stop`.
  - `example.c` is a Hello World C program to test the pass on
  - `example.cc` is a Hello World C++ program with some OO features to
    see what kinds of calls are visible after lowering to LLVM IR.

The following instructions assume `TAU_Profiling.so` and
`Tau_Profiling_CXX.so` have been built against the system installed
LLVM. If this is not the case, replace invocations of `clang` and
`clang++` with the appropriate versions.

If used, the runtime library must be compiled first (producing a shared
library is also OK):

``` bash
clang -c rtlib.c
# produces rtlib.o
```

To compile and link the example C program with the plugin and TAU
profiling:

``` bash
clang -fplugin=path/to/TAU_Profiling.so \
  -mllvm -tau-input-file=./functions_C.txt \
  -ldl -L path/to/TAU/x86_64/lib/shared -l TAU \
  -Wl,-rpath,path/to/TAU/x86_64/lib/shared \
  example.c
```

Linking against \`libdl\` is required for TAU. Specifying the path for
dynamic linking also appears to be necessary.

The process is similar for the example C++ program:

``` bash
clang -fplugin=path/to/TAU_Profiling_CXX.so \
  -mllvm -tau-input-file=./functions_CXX.txt \
  -ldl -L path/to/TAU/x86_64/lib/shared -l TAU \
  -Wl,-rpath,path/to/TAU/x86_64/lib/shared \
  example.cc
```

Running the resulting executable in either case should produce a
`profile.*` file.

## <span class="todo TODO">TODO</span> 

  - Write something to spit out the names of known called functions,
    demangled if possible/necessary. This will help the user know
    exactly what name of the function to use to make sure it's
    instrumented.
  - Look into regexes, maybe? Having to write the fully-qualified name
    of all the functions requiring instrumentation sounds tedious and
    error-prone.
  - Give better output about what is being instrumented.

## TOTHINK

### Where to insert calls

Profiling function calls are currently inserted around call sites. But
they could be inserted at function entry and exit (or it could be a
plugin parameter).

1.  Entry/Exit Pros
    
      - If I were doing it manually, that's what I'd do.
      - Presumably less noise in the IR, if ever inspected.
      - Can produce an instrumented library that just needs to be linked
        properly. This would be particularly useful for profiling across
        several apps using the same library.

2.  Entry/Exit Cons
    
      - Can't profile library calls (I think?) if all I have is the
        `.so` or `.a`, which may be a more realistic use-case.
      - Without better knowledge of IR function structure, it's not
        clear whether preserving semantics (esp. at function exit) is
        difficult.

## References

  - [Writing an LLVM Pass](http://llvm.org/docs/WritingAnLLVMPass.html)
  - [Adrian Sampson's LLVM pass guide
    (2015)](https://www.cs.cornell.edu/~asampson/blog/llvm.html)
