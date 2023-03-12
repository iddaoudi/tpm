#include <papi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "omp.h"
#include <pthread.h>

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

  // Turn on PAPI
  PAPI_library_init(PAPI_VER_CURRENT);
  // Initialize thread support
  if (PAPI_thread_init(pthread_self) != PAPI_OK) {
    printf("PAPI_thread_init problem\n");
    exit(1);
  }

  int available_threads = omp_get_max_threads();
  int events[NEVENTS] = {PAPI_L1_TCM, PAPI_L2_TCM, PAPI_L3_TCM,
                         PAPI_TLB_DM, PAPI_LD_INS, PAPI_SR_INS};
  long long values_by_thread[available_threads][NEVENTS];
  memset(values_by_thread, 0, sizeof(values_by_thread));

#pragma omp parallel
#pragma omp master
  {
    for (int th = 0; th < 10 * available_threads; th++) {
#pragma omp task firstprivate(a, b) shared(values_by_thread)
      {
        long long values[6];
        memset(values, 0, sizeof(values));
        int eventset = PAPI_NULL;
        int events[NEVENTS] = {PAPI_L1_TCM, PAPI_L2_TCM, PAPI_L3_TCM,
                               PAPI_TLB_DM, PAPI_LD_INS, PAPI_SR_INS};
        if (PAPI_create_eventset(&eventset) != PAPI_OK) {
          printf("PAPI_create_eventset error\n");
          exit(1);
        }
        PAPI_add_events(eventset, events, NEVENTS);

        // Start the counters
        PAPI_start(eventset);
        for (int j = 0; j < REPS; j++) {
          for (int i = 0; i < N; i++) {
            b[i] = a[i];
          }
        }
        PAPI_stop(eventset, values);

        for (int i = 0; i < NEVENTS; i++) {
          char event_name[PAPI_MAX_STR_LEN];
          PAPI_event_code_to_name(events[i], event_name);
#pragma omp atomic update
          values_by_thread[omp_get_thread_num()][i] += values[i];
#ifdef LOG
          printf("Thread %d: %s = %lld / %lld\n", omp_get_thread_num(),
                 event_name, values[i],
                 values_by_thread[omp_get_thread_num()][i]);
#endif
        }
        PAPI_unregister_thread();
      }
    }
  }

  PAPI_shutdown();

  free(a);
  free(b);

  // Compute cache miss ratio for each thread
  for (int i = 0; i < available_threads; i++) {
    double thread_c_misses =
        (long long)(values_by_thread[i][0] + values_by_thread[i][1] +
                    values_by_thread[i][2] + values_by_thread[i][3]);
    double thread_m_accesses =
        (long long)(values_by_thread[i][4] + values_by_thread[i][5]);
    double thread_ratio = thread_c_misses / thread_m_accesses;
#ifdef LOG
    printf("Total cache misses for thread %d: %f\n", i, thread_c_misses);
    printf("Total memory accesses for thread %d: %f\n", i, thread_m_accesses);
    printf("Cache miss ratio for thread %d: %f\n", i, thread_ratio);
#endif
  }

  long long total_values[NEVENTS];
  memset(total_values, 0, sizeof(total_values));
  for (int i = 0; i < NEVENTS; i++) {
    for (int j = 0; j < available_threads; j++) {
      total_values[i] += values_by_thread[j][i];
    }
  }
  // Compute cache miss ratio for all threads
  double total_c_misses = (long long)(total_values[0] + total_values[1] +
                                      total_values[2] + total_values[3]);
  double total_m_accesses = (long long)(total_values[4] + total_values[5]);
  double total_ratio = total_c_misses / total_m_accesses;
#ifdef LOG
  for (int i = 0; i < NEVENTS; i++) {
    char event_name[PAPI_MAX_STR_LEN];
    PAPI_event_code_to_name(events[i], event_name);
    printf("%s = %lld\n", event_name, values_by_thread[i] / REPS);
  }
  printf("Total cache misses for all threads: %f\n", c_misses);
  printf("Total memory accesses for all threads: %f\n", m_accesses);
#endif
  printf("Cache miss ratio for all threads: %f\n", total_ratio);

  return 0;
}
