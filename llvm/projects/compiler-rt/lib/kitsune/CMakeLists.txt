add_custom_target(kitsune)

set(KITSUNE_RTL_CFLAGS -std=c++11)
append_rtti_flag(OFF KITSUNE_RTL_CFLAGS)

# TODO: fix hardcoded CUDA path
include_directories(../../include /usr/local/cuda/include)

set(KITSUNE_SOURCES
    CUDARuntime.cpp
    GPURuntime.cpp)

foreach (arch ${KITSUNE_SUPPORTED_ARCH})
  add_compiler_rt_runtime(clang_rt.kitsune
    STATIC
    ARCHS ${arch}
    SOURCES ${KITSUNE_SOURCES}
    CFLAGS ${KITSUNE_RTL_CFLAGS}
    PARENT_TARGET kitsune)
  add_dependencies(kitsune
    clang_rt.kitsune-${arch})
endforeach()

add_dependencies(compiler-rt kitsune)
