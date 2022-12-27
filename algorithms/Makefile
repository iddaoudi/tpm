# OMPT doesn't seem to be working properly with clang-15
# We will only be using clang-12 for now, until we inverstigate further about compiler compatibilities

ROOT=/home/xps/Argonne/Software
tpm_TRACE_LIB=${ROOT}/tpm/trace/libtpm_trace.so
LLVM_OMP_LIB=${ROOT}/llvm-project/build-openmp

CC=clang
CXX=clang++

CFLAGS+=-std=gnu99 -ggdb -rdynamic -ldl
FLAGS_OPENMP=-fopenmp
OPENMP_LIBRARY=-lomp -I$(LLVM_OMP_LIB)/include -L$(LLVM_OMP_LIB)/lib

LAPACKE_LIB=$(shell pkg-config --libs openblas) $(shell pkg-config --libs lapacke)

all: tpm_dense

tpm_dense: main.c $(wildcard include/*.h dense/*.h dense/srcqr/*.h dense/srclu/*.h)
	$(CC) $(CFLAGS) $(FLAGS_OPENMP) $< -o $@ $(OPENMP_LIBRARY) $(LAPACKE_LIB)

.PHONY: all tpm_dense clean

clean:
	$(RM) tpm_dense *~
