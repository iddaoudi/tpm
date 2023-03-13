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

  int events[NEVENTS] = {PAPI_L1_TCM, PAPI_L2_TCM, PAPI_L3_TCM,
                         PAPI_TLB_DM, PAPI_LD_INS, PAPI_SR_INS};
  int available_threads = omp_get_max_threads();

  long long values_by_thread_potrf[available_threads][NEVENTS];
  long long values_by_thread_trsm[available_threads][NEVENTS];
  long long values_by_thread_syrk[available_threads][NEVENTS];
  long long values_by_thread_gemm[available_threads][NEVENTS];
  memset(values_by_thread_potrf, 0, sizeof(values_by_thread_potrf));
  memset(values_by_thread_trsm, 0, sizeof(values_by_thread_trsm));
  memset(values_by_thread_syrk, 0, sizeof(values_by_thread_syrk));
  memset(values_by_thread_gemm, 0, sizeof(values_by_thread_gemm));

  for (k = 0; k < A.matrix_size / A.tile_size; k++) {
    double *tileA = A(k, k);

#pragma omp task shared(values_by_thread_potrf)                                \
    depend(inout                                                               \
           : tileA [0:A.tile_size * A.tile_size])
    {
      long long values[NEVENTS];
      memset(values, 0, sizeof(values));
      int eventset = PAPI_NULL;
      int events[NEVENTS] = {PAPI_L1_TCM, PAPI_L2_TCM, PAPI_L3_TCM,
                             PAPI_TLB_DM, PAPI_LD_INS, PAPI_SR_INS};
      int ret = PAPI_create_eventset(&eventset);
      if (ret != PAPI_OK) {
        printf("POTRF - PAPI_create_eventset error %d: %s\n", ret, PAPI_strerror(ret));
        exit(1);
      }
      PAPI_add_events(eventset, events, NEVENTS);

      // Start the counters
      PAPI_start(eventset);
      LAPACKE_dpotrf(LAPACK_COL_MAJOR, 'U', A.tile_size, tileA, A.tile_size);
      PAPI_stop(eventset, values);

      for (int i = 0; i < NEVENTS; i++) {
#pragma omp atomic update
        values_by_thread_potrf[omp_get_thread_num()][i] += values[i];
      }
      PAPI_unregister_thread();
    }
    for (m = k + 1; m < A.matrix_size / A.tile_size; m++) {
      double *tileA = A(k, k);
      double *tileB = A(k, m);

#pragma omp task shared(values_by_thread_trsm)                                 \
    depend(in                                                                  \
           : tileA [0:A.tile_size * A.tile_size])                              \
        depend(inout                                                           \
               : tileB [0:A.tile_size * A.tile_size])
      {
        long long values[NEVENTS];
        memset(values, 0, sizeof(values));
        int eventset = PAPI_NULL;
        int events[NEVENTS] = {PAPI_L1_TCM, PAPI_L2_TCM, PAPI_L3_TCM,
                               PAPI_TLB_DM, PAPI_LD_INS, PAPI_SR_INS};
        if (PAPI_create_eventset(&eventset) != PAPI_OK) {
          printf("TRSM - PAPI_create_eventset error\n");
          exit(1);
        }
        PAPI_add_events(eventset, events, NEVENTS);

        // Start the counters
        PAPI_start(eventset);
        cblas_dtrsm(CblasColMajor, CblasLeft, CblasUpper, CblasTrans,
                    CblasNonUnit, A.tile_size, A.tile_size, 1.0, tileA,
                    A.tile_size, tileB, A.tile_size);
        PAPI_stop(eventset, values);

        for (int i = 0; i < NEVENTS; i++) {
#pragma omp atomic update
          values_by_thread_trsm[omp_get_thread_num()][i] += values[i];
        }
        PAPI_unregister_thread();
      }
    }

    for (m = k + 1; m < A.matrix_size / A.tile_size; m++) {
      double *tileA = A(k, m);
      double *tileB = A(m, m);

#pragma omp task shared(values_by_thread_syrk)                                 \
    depend(in                                                                  \
           : tileA [0:A.tile_size * A.tile_size])                              \
        depend(inout                                                           \
               : tileB [0:A.tile_size * A.tile_size])
      {
        long long values[NEVENTS];
        memset(values, 0, sizeof(values));
        int eventset = PAPI_NULL;
        int events[NEVENTS] = {PAPI_L1_TCM, PAPI_L2_TCM, PAPI_L3_TCM,
                               PAPI_TLB_DM, PAPI_LD_INS, PAPI_SR_INS};
        if (PAPI_create_eventset(&eventset) != PAPI_OK) {
          printf("SYRK - PAPI_create_eventset error\n");
          exit(1);
        }
        PAPI_add_events(eventset, events, NEVENTS);

        // Start the counters
        PAPI_start(eventset);
        cblas_dsyrk(CblasColMajor, CblasUpper, CblasTrans, A.tile_size,
                    A.tile_size, -1.0, tileA, A.tile_size, 1.0, tileB,
                    A.tile_size);
        PAPI_stop(eventset, values);

        for (int i = 0; i < NEVENTS; i++) {
#pragma omp atomic update
          values_by_thread_syrk[omp_get_thread_num()][i] += values[i];
        }
        PAPI_unregister_thread();
      }

      for (n = k + 1; n < m; n++) {
        double *tileA = A(k, n);
        double *tileB = A(k, m);
        double *tileC = A(n, m);

#pragma omp task shared(values_by_thread_gemm)                                 \
    depend(in                                                                  \
           : tileA [0:A.tile_size * A.tile_size],                              \
             tileB [0:A.tile_size * A.tile_size])                              \
        depend(inout                                                           \
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
          cblas_dgemm(CblasColMajor, CblasTrans, CblasNoTrans, A.tile_size,
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

  long long total_values_potrf[NEVENTS];
  long long total_values_trsm[NEVENTS];
  long long total_values_syrk[NEVENTS];
  long long total_values_gemm[NEVENTS];

  memset(total_values_potrf, 0, sizeof(total_values_potrf));
  memset(total_values_trsm, 0, sizeof(total_values_trsm));
  memset(total_values_syrk, 0, sizeof(total_values_syrk));
  memset(total_values_gemm, 0, sizeof(total_values_gemm));

  for (int i = 0; i < NEVENTS; i++) {
    for (int j = 0; j < available_threads; j++) {
      total_values_potrf[i] += values_by_thread_potrf[j][i];
      total_values_trsm[i] += values_by_thread_trsm[j][i];
      total_values_syrk[i] += values_by_thread_syrk[j][i];
      total_values_gemm[i] += values_by_thread_gemm[j][i];
    }
  }

  double total_c_misses_potrf = (double)(total_values_potrf[0] + total_values_potrf[1] + total_values_potrf[2] + total_values_potrf[3]);
  double total_c_misses_trsm = (double)(total_values_trsm[0] + total_values_trsm[1] + total_values_trsm[2] + total_values_trsm[3]);
  double total_c_misses_syrk = (double)(total_values_syrk[0] + total_values_syrk[1] + total_values_syrk[2] + total_values_syrk[3]);
  double total_c_misses_gemm = (double)(total_values_gemm[0] + total_values_gemm[1] + total_values_gemm[2] + total_values_gemm[3]);

  double total_m_accesses_potrf = (double)((total_values_potrf[4] + total_values_potrf[5]));
  double total_m_accesses_trsm = (double)((total_values_trsm[4] + total_values_trsm[5]));
  double total_m_accesses_syrk = (double)((total_values_syrk[4] + total_values_syrk[5]));
  double total_m_accesses_gemm = (double)((total_values_gemm[4] + total_values_gemm[5]));

  double total_ratio_potrf = total_c_misses_potrf / total_m_accesses_potrf;
  double total_ratio_trsm = total_c_misses_trsm / total_m_accesses_trsm;
  double total_ratio_syrk = total_c_misses_syrk / total_m_accesses_syrk;
  double total_ratio_gemm = total_c_misses_gemm / total_m_accesses_gemm;

  printf("Total cache misses potrf: %f\n", total_c_misses_potrf);
  printf("Total cache misses trsm: %f\n", total_c_misses_trsm);
  printf("Total cache misses syrk: %f\n", total_c_misses_syrk);
  printf("Total cache misses gemm: %f\n", total_c_misses_gemm);

  printf("Total memory accesses potrf: %f\n", total_m_accesses_potrf);
  printf("Total memory accesses trsm: %f\n", total_m_accesses_trsm);
  printf("Total memory accesses syrk: %f\n", total_m_accesses_syrk);
  printf("Total memory accesses gemm: %f\n", total_m_accesses_gemm);

  printf("Cache miss ratio potrf: %f\n", total_ratio_potrf);
  printf("Cache miss ratio trsm: %f\n", total_ratio_trsm);
  printf("Cache miss ratio syrk: %f\n", total_ratio_syrk);
  printf("Cache miss ratio gemm: %f\n", total_ratio_gemm);

}
