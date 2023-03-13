/*
 * =====================================================================================
 *
 *       Filename:  qr.h
 *
 *    Description:  Task-based QR algorithm
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

void qr(tpm_desc A, tpm_desc S) {
  int k = 0, m = 0, n = 0;

  int events[NEVENTS] = {PAPI_L1_TCM, PAPI_L2_TCM, PAPI_L3_TCM,
                         PAPI_TLB_DM, PAPI_LD_INS, PAPI_SR_INS};
  int available_threads = omp_get_max_threads();

  long long values_by_thread_geqrt[available_threads][NEVENTS];
  long long values_by_thread_ormqr[available_threads][NEVENTS];
  long long values_by_thread_tsqrt[available_threads][NEVENTS];
  long long values_by_thread_tsmqr[available_threads][NEVENTS];
  memset(values_by_thread_geqrt, 0, sizeof(values_by_thread_geqrt));
  memset(values_by_thread_ormqr, 0, sizeof(values_by_thread_ormqr));
  memset(values_by_thread_tsqrt, 0, sizeof(values_by_thread_tsqrt));
  memset(values_by_thread_tsmqr, 0, sizeof(values_by_thread_tsmqr));

  for (k = 0; k < A.matrix_size / A.tile_size; k++) {
    double *tileA = A(k, k);
    double *tileS = S(k, k);

#pragma omp task shared(values_by_thread_geqrt) depend(inout                                                  \
                        : tileA [0:S.tile_size * S.tile_size])                 \
    depend(out                                                                 \
           : tileS [0:A.tile_size * S.tile_size])
    {
      double tho[S.tile_size];
      double work[S.tile_size * S.tile_size];

      long long values[NEVENTS];
      memset(values, 0, sizeof(values));
      int eventset = PAPI_NULL;
      int events[NEVENTS] = {PAPI_L1_TCM, PAPI_L2_TCM, PAPI_L3_TCM,
                             PAPI_TLB_DM, PAPI_LD_INS, PAPI_SR_INS};
      int ret = PAPI_create_eventset(&eventset);
      if (ret != PAPI_OK) {
        printf("GEQRT - PAPI_create_eventset error %d: %s\n", ret, PAPI_strerror(ret));
        exit(1);
      }
      PAPI_add_events(eventset, events, NEVENTS);

      // Start the counters
      PAPI_start(eventset);
      tpm_dgeqrt(A.tile_size, S.tile_size, tileA, A.tile_size, tileS,
                 S.tile_size, &tho[0], &work[0]);
      PAPI_stop(eventset, values);

      for (int i = 0; i < NEVENTS; i++) {
#pragma omp atomic update
        values_by_thread_geqrt[omp_get_thread_num()][i] += values[i];
      }
      PAPI_unregister_thread();
    }
    for (n = k + 1; n < A.matrix_size / A.tile_size; n++) {
      double *tileA = A(k, k);
      double *tileS = S(k, k);
      double *tileB = A(k, n);

#pragma omp task shared(values_by_thread_ormqr) depend(in                                                     \
                        : tileA [0:S.tile_size * S.tile_size],                 \
                          tileS [0:A.tile_size * S.tile_size])                 \
    depend(inout                                                               \
           : tileB [0:S.tile_size * S.tile_size])
      {
        double work[S.tile_size * S.tile_size];

        long long values[NEVENTS];
        memset(values, 0, sizeof(values));
        int eventset = PAPI_NULL;
        int events[NEVENTS] = {PAPI_L1_TCM, PAPI_L2_TCM, PAPI_L3_TCM,
                               PAPI_TLB_DM, PAPI_LD_INS, PAPI_SR_INS};
        if (PAPI_create_eventset(&eventset) != PAPI_OK) {
          printf("ORMQR - PAPI_create_eventset error\n");
          exit(1);
        }
        PAPI_add_events(eventset, events, NEVENTS);

        // Start the counters
        PAPI_start(eventset);
        tpm_dormqr(tpm_left, tpm_transpose, A.tile_size, tileA, A.tile_size,
                   tileS, S.tile_size, tileB, A.tile_size, &work[0],
                   S.tile_size);
        PAPI_stop(eventset, values);

        for (int i = 0; i < NEVENTS; i++) {
#pragma omp atomic update
          values_by_thread_ormqr[omp_get_thread_num()][i] += values[i];
        }
        PAPI_unregister_thread();
      }
    }
    for (m = k + 1; m < A.matrix_size / A.tile_size; m++) {
      double *tileA = A(k, k);
      double *tileS = S(m, k);
      double *tileB = A(m, k);

#pragma omp task shared(values_by_thread_tsqrt) depend(inout                                                  \
                        : tileA [0:S.tile_size * S.tile_size],                 \
                          tileB [0:S.tile_size * S.tile_size])                 \
    depend(out                                                                 \
           : tileS [0:S.tile_size * A.tile_size])
      {
        double work[S.tile_size * S.tile_size];
        double tho[S.tile_size];

        long long values[NEVENTS];
        memset(values, 0, sizeof(values));
        int eventset = PAPI_NULL;
        int events[NEVENTS] = {PAPI_L1_TCM, PAPI_L2_TCM, PAPI_L3_TCM,
                               PAPI_TLB_DM, PAPI_LD_INS, PAPI_SR_INS};
        if (PAPI_create_eventset(&eventset) != PAPI_OK) {
          printf("TRQRT - PAPI_create_eventset error\n");
          exit(1);
        }
        PAPI_add_events(eventset, events, NEVENTS);

        // Start the counters
        PAPI_start(eventset);

        tpm_dtsqrt(A.tile_size, tileA, A.tile_size, tileB, A.tile_size, tileS,
                   S.tile_size, &tho[0], &work[0]);
        PAPI_stop(eventset, values);

        for (int i = 0; i < NEVENTS; i++) {
#pragma omp atomic update
          values_by_thread_tsqrt[omp_get_thread_num()][i] += values[i];
        }
        PAPI_unregister_thread();
      }
      for (n = k + 1; n < A.matrix_size / A.tile_size; n++) {
        double *tileA = A(k, n);
        double *tileS = S(m, k);
        double *tileB = A(m, n);
        double *tileC = A(m, k);

#pragma omp task shared(values_by_thread_tsmqr) depend(inout                                                  \
                        : tileA [0:S.tile_size * S.tile_size],                 \
                          tileB [0:S.tile_size * S.tile_size])                 \
    depend(in                                                                  \
           : tileC [0:S.tile_size * S.tile_size],                              \
             tileS [0:A.tile_size * S.tile_size])
        {
          double work[S.tile_size * S.tile_size];

          long long values[NEVENTS];
          memset(values, 0, sizeof(values));
          int eventset = PAPI_NULL;
          int events[NEVENTS] = {PAPI_L1_TCM, PAPI_L2_TCM, PAPI_L3_TCM,
                                 PAPI_TLB_DM, PAPI_LD_INS, PAPI_SR_INS};
          if (PAPI_create_eventset(&eventset) != PAPI_OK) {
            printf("TSMQR - PAPI_create_eventset error\n");
            exit(1);
          }
          PAPI_add_events(eventset, events, NEVENTS);

          // Start the counters
          PAPI_start(eventset);
          tpm_dtsmqr(tpm_left, tpm_transpose, A.tile_size, A.tile_size,
                     A.tile_size, A.tile_size, tileA, A.tile_size, tileB,
                     A.tile_size, tileC, A.tile_size, tileS, S.tile_size,
                     &work[0], A.tile_size);
          PAPI_stop(eventset, values);

          for (int i = 0; i < NEVENTS; i++) {
#pragma omp atomic update
            values_by_thread_tsmqr[omp_get_thread_num()][i] += values[i];
          }
          PAPI_unregister_thread();
        }
      }
    }
  }

  PAPI_shutdown();

  long long total_values_geqrt[NEVENTS];
  long long total_values_ormqr[NEVENTS];
  long long total_values_tsqrt[NEVENTS];
  long long total_values_tsmqr[NEVENTS];

  memset(total_values_geqrt, 0, sizeof(total_values_geqrt));
  memset(total_values_ormqr, 0, sizeof(total_values_ormqr));
  memset(total_values_tsqrt, 0, sizeof(total_values_tsqrt));
  memset(total_values_tsmqr, 0, sizeof(total_values_tsmqr));

  for (int i = 0; i < NEVENTS; i++) {
    for (int j = 0; j < available_threads; j++) {
      total_values_geqrt[i] += values_by_thread_geqrt[j][i];
      total_values_ormqr[i] += values_by_thread_ormqr[j][i];
      total_values_tsqrt[i] += values_by_thread_tsqrt[j][i];
      total_values_tsmqr[i] += values_by_thread_tsmqr[j][i];
    }
  }

  double total_c_misses_geqrt = (double)(total_values_geqrt[0] + total_values_geqrt[1] + total_values_geqrt[2] + total_values_geqrt[3]);
  double total_c_misses_ormqr = (double)(total_values_ormqr[0] + total_values_ormqr[1] + total_values_ormqr[2] + total_values_ormqr[3]);
  double total_c_misses_tsqrt = (double)(total_values_tsqrt[0] + total_values_tsqrt[1] + total_values_tsqrt[2] + total_values_tsqrt[3]);
  double total_c_misses_tsmqr = (double)(total_values_tsmqr[0] + total_values_tsmqr[1] + total_values_tsmqr[2] + total_values_tsmqr[3]);

  double total_m_accesses_geqrt = (double)((total_values_geqrt[4] + total_values_geqrt[5]));
  double total_m_accesses_ormqr = (double)((total_values_ormqr[4] + total_values_ormqr[5]));
  double total_m_accesses_tsqrt = (double)((total_values_tsqrt[4] + total_values_tsqrt[5]));
  double total_m_accesses_tsmqr = (double)((total_values_tsmqr[4] + total_values_tsmqr[5]));

  double total_ratio_geqrt = total_c_misses_geqrt / total_m_accesses_geqrt;
  double total_ratio_ormqr = total_c_misses_ormqr / total_m_accesses_ormqr;
  double total_ratio_tsqrt = total_c_misses_tsqrt / total_m_accesses_tsqrt;
  double total_ratio_tsmqr = total_c_misses_tsmqr / total_m_accesses_tsmqr;

  printf("Total cache misses geqrt: %f\n", total_c_misses_geqrt);
  printf("Total cache misses ormqr: %f\n", total_c_misses_ormqr);
  printf("Total cache misses tsqrt: %f\n", total_c_misses_tsqrt);
  printf("Total cache misses tsmqr: %f\n", total_c_misses_tsmqr);

  printf("Total memory accesses geqrt: %f\n", total_m_accesses_geqrt);
  printf("Total memory accesses ormqr: %f\n", total_m_accesses_ormqr);
  printf("Total memory accesses tsqrt: %f\n", total_m_accesses_tsqrt);
  printf("Total memory accesses tsmqr: %f\n", total_m_accesses_tsmqr);

  printf("Cache miss ratio geqrt: %f\n", total_ratio_geqrt);
  printf("Cache miss ratio ormqr: %f\n", total_ratio_ormqr);
  printf("Cache miss ratio tsqrt: %f\n", total_ratio_tsqrt);
  printf("Cache miss ratio tsmqr: %f\n", total_ratio_tsmqr);
}
