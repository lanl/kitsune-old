#!/bin/bash

set -e

BUILD_DIR="${BUILD_DIR:-$(realpath ../../build)}"
printf 'Using "%s" as kitsune build dir\n' "$BUILD_DIR"

CLANG="$BUILD_DIR/llvm/bin/clang"
printf 'Using "%s" as clang\n' "$CLANG"

TAU_PLUGIN="$BUILD_DIR/llvm/lib/TAU_Profiling.so"
printf 'Using "%s" as the TAU plugin shared lib\n' "$TAU_PLUGIN"

FUNC_FILE="${FUNC_FILE:-./functions_C.txt}"
TAU_START="${TAU_START:-tau_prof_func_call}"
TAU_STOP="${TAU_STOP:-tau_prof_func_ret}"
RT_LIB="${RT_LIB:-./rtlib.o}"

printf 'Using "%s" as the file holding function names to profile\n' "$FUNC_FILE"
printf 'Using "%s" as the Tau_start function\n' "$TAU_START"
printf 'Using "%s" as the Tau_stop function\n' "$TAU_STOP"
printf 'Using "%s" as the runtime library\n' "$RT_LIB"


if nm "$RT_LIB" | grep -q "$TAU_START"
then
  printf 'Found "%s" in runtime lib\n' "$TAU_START"
else
  printf 'Could not find "%s" in runtime lib\n' "$TAU_START"
  exit 1
fi

if nm "$RT_LIB" | grep -q "$TAU_STOP"
then
  printf 'Found "%s" in runtime lib\n' "$TAU_STOP"
else
  printf 'Could not find "%s" in runtime lib\n' "$TAU_STOP"
  exit 1
fi


set -x

"$CLANG" -fplugin="$TAU_PLUGIN" \
         -mllvm -tau-input-file="$FUNC_FILE" \
         -mllvm -tau-start-func="$TAU_START" \
         -mllvm -tau-stop-func="$TAU_STOP" \
         example.c "$RT_LIB"
