# OMPT doesn't seem to be working properly with clang-15

# Path
ROOT=/home/xps/Work/Software

# Compiler
CC=clang
CXX=clang++

# C flags
CFLAGS+=-std=gnu99 -ggdb -rdynamic -ldl

# Trace library
TPM_TRACE_LIB=${ROOT}/tpm/trace/libtpm_trace.so

# OpenMP
LLVM_OMP_LIB=${ROOT}/llvm-project/build-openmp
OPENMP_LIBRARY=-lomp -I$(LLVM_OMP_LIB)/include -L$(LLVM_OMP_LIB)/lib

# OpenBLAS
LAPACKE_LIB=${ROOT}/OpenBLAS/build
LAPACKE_LIBRARY=-lopenblas -I${LAPACKE_LIB}/include -L${LAPACKE_LIB}/lib

all: tpm_benchmark

tpm_benchmark: main.c $(wildcard include/*.h dense/*.h dense/srcqr/*.h dense/srclu/*.h sparse/*.h srcslu/*.h)
	$(CC) $(CFLAGS) $< -o $@ $(OPENMP_LIBRARY) $(LAPACKE_LIBRARY)

.PHONY: all tpm_benchmark clean

clean:
	$(RM) tpm_dense *~
