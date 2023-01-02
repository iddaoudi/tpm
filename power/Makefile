CC=clang
CXX=clang++

CFLAGS+=-std=gnu99 -O2

ZMQ_LIB=-lzmq 
CPUFREQ_LIB=-lcpufreq

all: tpm_server

tpm_server: main.c $(wildcard include/*.h control/*.h)
	$(CC) $(CFLAGS) $< -o $@ $(ZMQ_LIB) $(CPUFREQ_LIB)

.PHONY: all tpm_server clean

clean:
	$(RM) tpm_server *~
