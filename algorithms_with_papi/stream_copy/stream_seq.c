#include <papi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 4194304
#define REPS 10
#define NEVENTS 6
//#define LOG 1

int main() {
  double *a, *b;
  if (posix_memalign((void **)&a, 64, N * sizeof(double)) != 0) {
    printf("Memory allocation problem for vector A\n");
  }
  if (posix_memalign((void **)&b, 64, N * sizeof(double)) != 0) {
    printf("Memory allocation problem for vector A\n");
  }

  for (int i = 0; i < N; i++) {
    a[i] = (double)rand() / RAND_MAX;
    b[i] = 0.0;
  }

  // Initialize PAPI
  PAPI_library_init(PAPI_VER_CURRENT);

  int events[NEVENTS] = {PAPI_L1_TCM, PAPI_L2_TCM, PAPI_L3_TCM,
                         PAPI_TLB_DM, PAPI_LD_INS, PAPI_SR_INS};
  int eventset = PAPI_NULL;
  if (PAPI_create_eventset(&eventset) != PAPI_OK) {
    printf("PAPI_create_eventset error\n");
    exit(1);
  }
  PAPI_add_events(eventset, events, NEVENTS);

  long long values[NEVENTS];

  // Start the counters
  PAPI_start(eventset);
  for (int j = 0; j < REPS; j++) {
    for (int i = 0; i < N; i++) {
      b[i] = a[i];
    }
  }
  PAPI_stop(eventset, values);

  // Compute cache miss ratio
  double c_misses = (long long)(values[0] + values[1] + values[2] + values[3]);
  double m_accesses = (long long)(values[4] + values[5]);
  double ratio = c_misses / m_accesses;
#ifdef LOG
  for (int i = 0; i < NEVENTS; i++) {
    char event_name[PAPI_MAX_STR_LEN];
    PAPI_event_code_to_name(events[i], event_name);
    printf("%s = %lld\n", event_name, values[i] / REPS);
  }
  printf("Total cache misses: %f\n", c_misses);
  printf("Total memory accesses: %f\n", m_accesses);
#endif
  printf("Cache miss ratio: %f\n", ratio);

  return 0;
}
