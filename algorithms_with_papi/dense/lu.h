/*
 * =====================================================================================
 *
 *       Filename:  lu.h
 *
 *    Description:  Task-based LU algorithm
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

void lu(tpm_desc A) {
  int k = 0, m = 0, n = 0;

  int events[NEVENTS] = {PAPI_L1_TCM, PAPI_L2_TCM, PAPI_L3_TCM,
                         PAPI_TLB_DM, PAPI_LD_INS, PAPI_SR_INS};
  int available_threads = omp_get_max_threads();

  long long values_by_thread_getrf[available_threads][NEVENTS];
  long long values_by_thread_trsm1[available_threads][NEVENTS];
  long long values_by_thread_trsm2[available_threads][NEVENTS];
  long long values_by_thread_gemm[available_threads][NEVENTS];
  memset(values_by_thread_getrf, 0, sizeof(values_by_thread_getrf));
  memset(values_by_thread_trsm1, 0, sizeof(values_by_thread_trsm1));
  memset(values_by_thread_trsm2, 0, sizeof(values_by_thread_trsm2));
  memset(values_by_thread_gemm, 0, sizeof(values_by_thread_gemm));

  for (k = 0; k < A.matrix_size / A.tile_size; k++) {
    double *tileA = A(k, k);

#pragma omp task shared(values_by_thread_getrf) depend(inout : tileA[0])
    {
      long long values[NEVENTS];
      memset(values, 0, sizeof(values));
      int eventset = PAPI_NULL;
      int events[NEVENTS] = {PAPI_L1_TCM, PAPI_L2_TCM, PAPI_L3_TCM,
                             PAPI_TLB_DM, PAPI_LD_INS, PAPI_SR_INS};
      int ret = PAPI_create_eventset(&eventset);
      if (ret != PAPI_OK) {
        printf("GETRF - PAPI_create_eventset error %d: %s\n", ret, PAPI_strerror(ret));
        exit(1);
      }
      PAPI_add_events(eventset, events, NEVENTS);

      // Start the counters
      PAPI_start(eventset);
      tpm_dgetrf(A.tile_size, tileA, A.tile_size);
      PAPI_stop(eventset, values);

      for (int i = 0; i < NEVENTS; i++) {
#pragma omp atomic update
        values_by_thread_getrf[omp_get_thread_num()][i] += values[i];
      }
      PAPI_unregister_thread();
    }
    for (m = k + 1; m < A.matrix_size / A.tile_size; m++) {
      double *tileA = A(k, k);
      double *tileB = A(m, k);

#pragma omp task shared(values_by_thread_trsm1) depend(in                                                     \
                        : tileA [0:A.tile_size * A.tile_size])                 \
    depend(inout                                                               \
           : tileB[A.tile_size * A.tile_size])
      {
        long long values[NEVENTS];
        memset(values, 0, sizeof(values));
        int eventset = PAPI_NULL;
        int events[NEVENTS] = {PAPI_L1_TCM, PAPI_L2_TCM, PAPI_L3_TCM,
                               PAPI_TLB_DM, PAPI_LD_INS, PAPI_SR_INS};
        if (PAPI_create_eventset(&eventset) != PAPI_OK) {
          printf("TRSM1 - PAPI_create_eventset error\n");
          exit(1);
        }
        PAPI_add_events(eventset, events, NEVENTS);

        // Start the counters
        PAPI_start(eventset);
        cblas_dtrsm(CblasColMajor, CblasRight, CblasUpper, CblasNoTrans,
                    CblasNonUnit, A.tile_size, A.tile_size, 1.0, tileA,
                    A.tile_size, tileB, A.tile_size);
        PAPI_stop(eventset, values);

        for (int i = 0; i < NEVENTS; i++) {
#pragma omp atomic update
          values_by_thread_trsm1[omp_get_thread_num()][i] += values[i];
        }
        PAPI_unregister_thread();
      }
    }
    for (n = k + 1; n < A.matrix_size / A.tile_size; n++) {
      double *tileA = A(k, k);
      double *tileB = A(k, n);

#pragma omp task shared(values_by_thread_trsm2) depend(in                                                     \
                        : tileA [0:A.tile_size * A.tile_size])                 \
    depend(inout                                                               \
           : tileB[A.tile_size * A.tile_size])
      {
        long long values[NEVENTS];
        memset(values, 0, sizeof(values));
        int eventset = PAPI_NULL;
        int events[NEVENTS] = {PAPI_L1_TCM, PAPI_L2_TCM, PAPI_L3_TCM,
                               PAPI_TLB_DM, PAPI_LD_INS, PAPI_SR_INS};
        if (PAPI_create_eventset(&eventset) != PAPI_OK) {
          printf("TRSM2 - PAPI_create_eventset error\n");
          exit(1);
        }
        PAPI_add_events(eventset, events, NEVENTS);

        // Start the counters
        PAPI_start(eventset);
        cblas_dtrsm(CblasColMajor, CblasLeft, CblasLower, CblasNoTrans,
                    CblasUnit, A.tile_size, A.tile_size, 1.0, tileA,
                    A.tile_size, tileB, A.tile_size);
        PAPI_stop(eventset, values);

        for (int i = 0; i < NEVENTS; i++) {
#pragma omp atomic update
          values_by_thread_trsm2[omp_get_thread_num()][i] += values[i];
        }
        PAPI_unregister_thread();
      }
      for (m = k + 1; m < A.matrix_size / A.tile_size; m++) {
        double *tileA = A(m, k);
        double *tileB = A(k, n);
        double *tileC = A(m, n);

#pragma omp task shared(values_by_thread_gemm) depend(in                                                     \
                        : tileA [0:A.tile_size * A.tile_size],                 \
                          tileB [0:A.tile_size * A.tile_size])                 \
    depend(inout                                                               \
           : tileC [0:A.tile_size * A.tile_size])
        {
          long long values[NEVENTS];
          memset(values, 0, sizeof(values));
          int eventset = PAPI_NULL;
          int events[NEVENTS] = {PAPI_L1_TCM, PAPI_L2_TCM, PAPI_L3_TCM,
                                 PAPI_TLB_DM, PAPI_LD_INS, PAPI_SR_INS};
          if (PAPI_create_eventset(&eventset) != PAPI_OK) {
            printf("GEMM - PAPI_create_eventset error\n");
            exit(1);
          }
          PAPI_add_events(eventset, events, NEVENTS);

          // Start the counters
          PAPI_start(eventset);
          cblas_dgemm(CblasColMajor, CblasNoTrans, CblasNoTrans, A.tile_size,
                      A.tile_size, A.tile_size, -1.0, tileA, A.tile_size, tileB,
                      A.tile_size, 1.0, tileC, A.tile_size);
          PAPI_stop(eventset, values);

          for (int i = 0; i < NEVENTS; i++) {
#pragma omp atomic update
            values_by_thread_gemm[omp_get_thread_num()][i] += values[i];
          }
          PAPI_unregister_thread();
        }
      }
    }
  }

  PAPI_shutdown();

  long long total_values_getrf[NEVENTS];
  long long total_values_trsm1[NEVENTS];
  long long total_values_trsm2[NEVENTS];
  long long total_values_gemm[NEVENTS];

  memset(total_values_getrf, 0, sizeof(total_values_getrf));
  memset(total_values_trsm1, 0, sizeof(total_values_trsm1));
  memset(total_values_trsm2, 0, sizeof(total_values_trsm2));
  memset(total_values_gemm, 0, sizeof(total_values_gemm));

  for (int i = 0; i < NEVENTS; i++) {
    for (int j = 0; j < available_threads; j++) {
      total_values_getrf[i] += values_by_thread_getrf[j][i];
      total_values_trsm1[i] += values_by_thread_trsm1[j][i];
      total_values_trsm2[i] += values_by_thread_trsm2[j][i];
      total_values_gemm[i] += values_by_thread_gemm[j][i];
    }
  }

  double total_c_misses_getrf = (double)(total_values_getrf[0] + total_values_getrf[1] + total_values_getrf[2] + total_values_getrf[3]);
  double total_c_misses_trsm1 = (double)(total_values_trsm1[0] + total_values_trsm1[1] + total_values_trsm1[2] + total_values_trsm1[3]);
  double total_c_misses_trsm2 = (double)(total_values_trsm2[0] + total_values_trsm2[1] + total_values_trsm2[2] + total_values_trsm2[3]);
  double total_c_misses_gemm = (double)(total_values_gemm[0] + total_values_gemm[1] + total_values_gemm[2] + total_values_gemm[3]);

  double total_m_accesses_getrf = (double)((total_values_getrf[4] + total_values_getrf[5]));
  double total_m_accesses_trsm1 = (double)((total_values_trsm1[4] + total_values_trsm1[5]));
  double total_m_accesses_trsm2 = (double)((total_values_trsm2[4] + total_values_trsm2[5]));
  double total_m_accesses_gemm = (double)((total_values_gemm[4] + total_values_gemm[5]));

  double total_ratio_getrf = total_c_misses_getrf / total_m_accesses_getrf;
  double total_ratio_trsm1 = total_c_misses_trsm1 / total_m_accesses_trsm1;
  double total_ratio_trsm2 = total_c_misses_trsm2 / total_m_accesses_trsm2;
  double total_ratio_gemm = total_c_misses_gemm / total_m_accesses_gemm;

  printf("Total cache misses getrf: %f\n", total_c_misses_getrf);
  printf("Total cache misses trsm1: %f\n", total_c_misses_trsm1);
  printf("Total cache misses trsm2: %f\n", total_c_misses_trsm2);
  printf("Total cache misses gemm: %f\n", total_c_misses_gemm);

  printf("Total memory accesses getrf: %f\n", total_m_accesses_getrf);
  printf("Total memory accesses trsm1: %f\n", total_m_accesses_trsm1);
  printf("Total memory accesses trsm2: %f\n", total_m_accesses_trsm2);
  printf("Total memory accesses gemm: %f\n", total_m_accesses_gemm);

  printf("Cache miss ratio getrf: %f\n", total_ratio_getrf);
  printf("Cache miss ratio trsm1: %f\n", total_ratio_trsm1);
  printf("Cache miss ratio trsm2: %f\n", total_ratio_trsm2);
  printf("Cache miss ratio gemm: %f\n", total_ratio_gemm);
}
