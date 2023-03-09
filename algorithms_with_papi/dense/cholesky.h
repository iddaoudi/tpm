/*
 * =====================================================================================
 *
 *       Filename:  cholesky.h
 *
 *    Description:  Task-based Choklesky algorithm
 *
 *        Version:  1.0
 *        Created:  25/12/2022 19:30:35
 *       Revision:  none
 *       Compiler:  clang
 *
 *         Author:  Idriss Daoudi <idaoudi@anl.gov>
 *   Organization:  Argonne National Laboratory
 *
 * =====================================================================================
 */

#include <papi.h>

//void cholesky(tpm_desc A, int eventset, int events[], long long values[], int task_count) {
void cholesky(tpm_desc A) {
  int k = 0, m = 0, n = 0;
  
  int retval = PAPI_library_init(PAPI_VER_CURRENT);
  if (retval != PAPI_VER_CURRENT) {
  	printf("PAPI library init error!\n");
        exit(1);
  }
  int events[6] = {PAPI_L1_TCM, PAPI_L2_TCM, PAPI_L3_TCM, PAPI_TLB_DM, PAPI_LD_INS, PAPI_SR_INS};
  int eventset = PAPI_NULL;
  retval = PAPI_create_eventset(&eventset);
  if (retval != PAPI_OK) {
  	printf("PAPI_create_eventset error: %s\n", PAPI_strerror(retval));
        exit(1);
  }
  PAPI_add_events(eventset, events, 6);
  int task_count = 0;
  int max_threads = omp_get_num_threads();
  long long values_by_thread[max_threads][6];
  memset(values_by_thread, 0, max_threads * 6 * sizeof(long long));

  for (k = 0; k < A.matrix_size / A.tile_size; k++) {
    double *tileA = A(k, k);

#pragma omp task shared(task_count, values_by_thread, eventset)                              \
    depend(inout                                                               \
           : tileA [0:A.tile_size * A.tile_size])
    {
        long long values[6] = {0};
	PAPI_start(eventset);
	LAPACKE_dpotrf(LAPACK_COL_MAJOR, 'U', A.tile_size, tileA, A.tile_size);
        PAPI_stop(eventset, values);
#pragma omp atomic update
	task_count++;
	int thread_num = omp_get_thread_num();
	for (int i = 0; i < 6; i++) {
#pragma omp atomic update
		values_by_thread[thread_num][i] += values[i];
	}
	printf("task count inside: %d\n", task_count);
    	printf("values 0: %lld\n", values[0]);
    }
    for (m = k + 1; m < A.matrix_size / A.tile_size; m++) {
      double *tileA = A(k, k);
      double *tileB = A(k, m);

#pragma omp task                               \
    depend(in                                                                  \
           : tileA [0:A.tile_size * A.tile_size])                              \
        depend(inout                                                           \
               : tileB [0:A.tile_size * A.tile_size])
      {
        cblas_dtrsm(CblasColMajor, CblasLeft, CblasUpper, CblasTrans,
                    CblasNonUnit, A.tile_size, A.tile_size, 1.0, tileA,
                    A.tile_size, tileB, A.tile_size);
      }
    }

    for (m = k + 1; m < A.matrix_size / A.tile_size; m++) {
      double *tileA = A(k, m);
      double *tileB = A(m, m);

#pragma omp task                               \
    depend(in                                                                  \
           : tileA [0:A.tile_size * A.tile_size])                              \
        depend(inout                                                           \
               : tileB [0:A.tile_size * A.tile_size])
      {
        cblas_dsyrk(CblasColMajor, CblasUpper, CblasTrans, A.tile_size,
                    A.tile_size, -1.0, tileA, A.tile_size, 1.0, tileB,
                    A.tile_size);
      }
      
		for (n = k + 1; n < m; n++) {
        double *tileA = A(k, n);
        double *tileB = A(k, m);
        double *tileC = A(n, m);

#pragma omp task                               \
    depend(in                                                                  \
           : tileA [0:A.tile_size * A.tile_size],                              \
             tileB [0:A.tile_size * A.tile_size])                              \
        depend(inout                                                           \
               : tileC [0:A.tile_size * A.tile_size])
        {
          cblas_dgemm(CblasColMajor, CblasTrans, CblasNoTrans, A.tile_size,
                      A.tile_size, A.tile_size, -1.0, tileA, A.tile_size, tileB,
                      A.tile_size, 1.0, tileC, A.tile_size);
        }
      }
    }
  }
#pragma omp taskwait
  long long final_values[6] = {0};
  for (int i = 0; i < max_threads; i++) {
  	for (int j = 0; j < 6; j++) {
		final_values[j] += values_by_thread[i][j];
	}
  }
  PAPI_cleanup_eventset(eventset);
  double c_misses = (double) (final_values[0] + final_values[1] + final_values[2] + final_values[3]) / task_count;
  double m_accesses = (double) ((final_values[4] + final_values[5]) / task_count) / sizeof(double);
  double ratio = c_misses / m_accesses;
  printf("Task count: %d\n", task_count);
  printf("Total cache misses: %f\n", c_misses);
  printf("Total memory accesses: %f\n", m_accesses);
  printf("Cache miss ratio: %f\n", ratio);
}
