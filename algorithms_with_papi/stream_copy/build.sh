#!/bin/bash

file=$1

export OMP_NUM_THREADS=32
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/cc/llvm-project/build-openmp/lib

gcc -o stream $file -O2 -lpapi -mcmodel=large -fopenmp -L/home/cc/llvm-project/build-openmp/lib -lomp
