#include <stdlib.h>
#include <stdio.h>
#include <papi.h>
#include "omp.h"

#define N 1e9

int main() {
    double *a, *b;
    long long values[6];

    if (posix_memalign((void **)&a, 64, N * sizeof(double)) != 0) {
        printf("Memory allocation problem for vector A\n");
    }
    if (posix_memalign((void **)&b, 64, N * sizeof(double)) != 0) {
        printf("Memory allocation problem for vector A\n");
    }

    for (int i = 0; i < N; i++) {
        a[i] = (double) rand() / RAND_MAX;
        b[i] = 0.0;
    }

    PAPI_library_init(PAPI_VER_CURRENT);
    if (PAPI_thread_init(omp_get_thread_num) != PAPI_OK) {
        printf("PAPI threads problem\n");
    }
    int events[6] = {PAPI_L1_TCM, PAPI_L2_TCM, PAPI_L3_TCM, PAPI_TLB_DM, PAPI_LD_INS, PAPI_SR_INS};
    int eventset = PAPI_NULL;
    int retval = PAPI_create_eventset(&eventset);
    if (retval != PAPI_OK) {
    	printf("PAPI_create_eventset error: %s\n", PAPI_strerror(retval));
    	exit(1);
    }
    PAPI_add_events(eventset, events, 6);

#pragma omp single
    {
        PAPI_start(eventset);
#pragma omp task
        {
            for (int i = 0; i < N; i++) {
                b[i] = a[i];
            }
        }
        PAPI_stop(eventset);
    }

    PAPI_cleanup_eventset(eventset);
    PAPI_destroy_eventset(&eventset);
    PAPI_shutdown();

    free(a);
    free(b);

    for (int i = 0; i < 6; i++) {
        printf("%s = %lld\n", events[i], values[i]);
    }

}
