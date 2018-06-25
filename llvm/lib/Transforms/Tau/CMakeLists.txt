# This copied/modified from the "Hello World" LLVM pass example (see
# lib/Transform/Hello)

# If we're building the pass alone, get the required LLVM stuff
if(TAU_PROF_STANDALONE)
  find_package(LLVM REQUIRED CONFIG)

  # required for `add_llvm_loadable_module`
  list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
  include(AddLLVM)

  add_definitions(${LLVM_DEFINITIONS})
  include_directories(${LLVM_INCLUDE_DIRS})
endif()


# If we don't need RTTI or EH, there's no reason to export anything
# from the hello plugin.
if( NOT LLVM_REQUIRES_RTTI )
  if( NOT LLVM_REQUIRES_EH )
    set(LLVM_EXPORTED_SYMBOL_FILE ${CMAKE_CURRENT_SOURCE_DIR}/Hello.exports)
  endif()
endif()

if(WIN32 OR CYGWIN)
  set(LLVM_LINK_COMPONENTS Core Support)
endif()

add_llvm_loadable_module( TAU_Profiling
  Instrument.cpp

  DEPENDS
  intrinsics_gen
  PLUGIN_TOOL
  opt
  )
