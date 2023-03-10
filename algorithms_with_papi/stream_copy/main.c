#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <papi.h>
#include <string.h>
#define N (1<<28)
#define REPS 11

int main() {

    long long values[6];

    // Memory allocation
    double *a, *b, *c;
    a = (double*) malloc(N * sizeof(double));
    b = (double*) malloc(N * sizeof(double));
    c = (double*) malloc(N * sizeof(double));

    
    // Initialize PAPI
    int retval = PAPI_library_init(PAPI_VER_CURRENT);
    if (retval != PAPI_VER_CURRENT) {
        printf("PAPI library init error!\n");
        exit(1);
    }

    // Add cache miss and memory access counters
    int events[6] = {PAPI_L1_TCM, PAPI_L2_TCM, PAPI_L3_TCM, PAPI_TLB_DM, PAPI_LD_INS, PAPI_SR_INS};
    int eventset = PAPI_NULL;
    retval = PAPI_create_eventset(&eventset);
    if (retval != PAPI_OK) {
    	printf("PAPI_create_eventset error: %s\n", PAPI_strerror(retval));
    	exit(1);
    }
    PAPI_add_events(eventset, events, 6);

    // Warm up caches
    for (int i = 0; i < N; i++) {
        a[i] = (double) rand() / RAND_MAX;
        b[i] = 0.0;
        c[i] = (double) rand() / RAND_MAX;
    }

    // Measure cache misses and memory accesses
    PAPI_start(eventset);
    for (int j = 0; j < REPS; j++) {
        //memcpy(b, a, sizeof(double) * N);
	 
	for (int i = 0; i < N; i++) {
            b[i] = a[i];
        }
    }
    PAPI_stop(eventset, values);

    // Compute cache miss ratio
    double c_misses = (double) (values[0]); // + values[1] + values[2] + values[3])/ REPS;
    double m_accesses = (double) (values[4] + values[5]);
    double ratio = c_misses / m_accesses;

    for (int i = 0; i < 6; i++) {
    	printf("values[ %d ] : %lld\n", i, values[i]);
    }
    
    printf("Total cache misses: %f\n", c_misses);
    printf("Total memory accesses: %f\n", m_accesses);
    printf("Cache miss ratio: %f\n", ratio);

    return 0;
}

