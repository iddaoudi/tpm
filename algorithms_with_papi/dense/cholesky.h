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

#define NEVENTS 6

void cholesky(tpm_desc A) {
  int k = 0, m = 0, n = 0;
  
  int events[NEVENTS] = {PAPI_L1_TCM, PAPI_L2_TCM, PAPI_L3_TCM, PAPI_TLB_DM, PAPI_LD_INS, PAPI_SR_INS};
  int available_threads = omp_get_max_threads();
  
  long long values_by_thread[available_threads][NEVENTS];
  memset(values_by_thread, 0, sizeof(values_by_thread));
  
  for (k = 0; k < A.matrix_size / A.tile_size; k++) {
    double *tileA = A(k, k);

#pragma omp task shared(values_by_thread)                              \
    depend(inout                                                               \
           : tileA [0:A.tile_size * A.tile_size])
    {
        long long values[NEVENTS];
	    memset(values, 0, sizeof(values));
        int eventset = PAPI_NULL;
        int events[NEVENTS] = {PAPI_L1_TCM, PAPI_L2_TCM, PAPI_L3_TCM, PAPI_TLB_DM, PAPI_LD_INS, PAPI_SR_INS};
        if (PAPI_create_eventset(&eventset) != PAPI_OK) {
            printf("PAPI_create_eventset error\n");
            exit(1);
        }
        PAPI_add_events(eventset, events, NEVENTS);
        
        // Start the counters
        PAPI_start(eventset);
	    LAPACKE_dpotrf(LAPACK_COL_MAJOR, 'U', A.tile_size, tileA, A.tile_size);
        PAPI_stop(eventset, values);
    
        for (int i = 0; i < NEVENTS; i++) {
#pragma omp atomic
            values_by_thread[omp_get_thread_num()][i] += values[i];
        }
        PAPI_unregister_thread();
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
 
  PAPI_shutdown();
  long long total_values[NEVENTS];
  memset(total_values, 0, sizeof(total_values));
  for (int i = 0; i < NEVENTS; i++) {
      for (int j = 0; j < available_threads; j++) {
          total_values[i] += values_by_thread[j][i];
      }
  }

  double total_c_misses = (double) (total_values[0] + total_values[1] + total_values[2] + total_values[3]);
  double total_m_accesses = (double) ((total_values[4] + total_values[5]));
  double total_ratio = total_c_misses / total_m_accesses;
  printf("Total cache misses: %f\n", total_c_misses);
  printf("Total memory accesses: %f\n", total_m_accesses);
  printf("Cache miss ratio: %f\n", total_ratio);
}
