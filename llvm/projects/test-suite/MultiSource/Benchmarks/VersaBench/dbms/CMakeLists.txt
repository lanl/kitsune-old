set(PROG dbms)
list(APPEND LDFLAGS -lm)
if(SMALL_PROBLEM_SIZE)
  set(RUN_OPTIONS -i ${CMAKE_CURRENT_SOURCE_DIR}/dbms.train.in)
else()
  set(RUN_OPTIONS -i ${CMAKE_CURRENT_SOURCE_DIR}/dbms.ref.in)
endif()
llvm_multisource()
