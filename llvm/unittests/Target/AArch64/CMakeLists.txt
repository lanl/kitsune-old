include_directories(
  ${LLVM_MAIN_SRC_DIR}/lib/Target/AArch64
  ${LLVM_BINARY_DIR}/lib/Target/AArch64
  )

set(LLVM_LINK_COMPONENTS
  AArch64CodeGen
  AArch64Desc
  AArch64Info
  CodeGen
  Core
  MC
  MIRParser
  SelectionDAG
  Support
  Target
  )

add_llvm_unittest(AArch64Tests
  InstSizes.cpp
  )
