#!/bin/bash
CLANG=/home/keiko/Projects/clang-llvm/build/bin/clang

$CLANG -Xclang -ast-dump -fsyntax-only -fdiagnostics-color=never "$@"
