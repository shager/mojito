#!/bin/sh

# test bitvector
cd ../src
cp bv_nojit.c bv_types.c
make clean
make

cd ../eval
./memory_executor.py "simple_bv"

# test jit
cd ../src
cp bv_jit.c bv_types.c
make clean
make

cd ../eval
./memory_executor.py "jit"
