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

#include "common.h"

void qr(tpm_desc A, tpm_desc S) {
  if (TPM_TRACE)
    tpm_downstream_start("qr", A.matrix_size, A.tile_size, NTH);
  int k = 0, m = 0, n = 0;
  for (k = 0; k < A.matrix_size / A.tile_size; k++) {
    double *tileA = A(k, k);
    double *tileS = S(k, k);

    char *name_with_id_char = tpm_unique_task_identifier("geqrt", k, m, n);
    if (TPM_TRACE)
      tpm_upstream_set_task_name(name_with_id_char);
#pragma omp task depend(inout                                                  \
                        : tileA [0:S.tile_size * S.tile_size])                 \
    depend(out                                                                 \
           : tileS [0:A.tile_size * S.tile_size])
    {
      double tho[S.tile_size];
      double work[S.tile_size * S.tile_size];
      struct timeval start, end;

      unsigned int cpu, node;
      getcpu(&cpu, &node);
      if (TPM_TRACE)
        tpm_upstream_set_task_cpu_node(cpu, node, name_with_id_char);

      gettimeofday(&start, NULL);
      tpm_dgeqrt(A.tile_size, S.tile_size, tileA, A.tile_size, tileS,
                 S.tile_size, &tho[0], &work[0]);
      gettimeofday(&end, NULL);

      if (TPM_TRACE)
        tpm_upstream_get_task_time(start, end, name_with_id_char);
    }
    for (n = k + 1; n < A.matrix_size / A.tile_size; n++) {
      double *tileA = A(k, k);
      double *tileS = S(k, k);
      double *tileB = A(k, n);

      char *name_with_id_char = tpm_unique_task_identifier("ormqr", k, m, n);
      if (TPM_TRACE)
        tpm_upstream_set_task_name(name_with_id_char);
#pragma omp task depend(in                                                     \
                        : tileA [0:S.tile_size * S.tile_size],                 \
                          tileS [0:A.tile_size * S.tile_size])                 \
    depend(inout                                                               \
           : tileB [0:S.tile_size * S.tile_size])
      {
        double work[S.tile_size * S.tile_size];
        struct timeval start, end;

        unsigned int cpu, node;
        getcpu(&cpu, &node);
        if (TPM_TRACE)
          tpm_upstream_set_task_cpu_node(cpu, node, name_with_id_char);

        gettimeofday(&start, NULL);
        tpm_dormqr(tpm_left, tpm_transpose, A.tile_size, tileA, A.tile_size,
                   tileS, S.tile_size, tileB, A.tile_size, &work[0],
                   S.tile_size);
        gettimeofday(&end, NULL);

        if (TPM_TRACE)
          tpm_upstream_get_task_time(start, end, name_with_id_char);
      }
    }
    for (m = k + 1; m < A.matrix_size / A.tile_size; m++) {
      double *tileA = A(k, k);
      double *tileS = S(m, k);
      double *tileB = A(m, k);

      char *name_with_id_char = tpm_unique_task_identifier("tsqrt", k, m, n);
      if (TPM_TRACE)
        tpm_upstream_set_task_name(name_with_id_char);
#pragma omp task depend(inout                                                  \
                        : tileA [0:S.tile_size * S.tile_size],                 \
                          tileB [0:S.tile_size * S.tile_size])                 \
    depend(out                                                                 \
           : tileS [0:S.tile_size * A.tile_size])
      {
        double work[S.tile_size * S.tile_size];
        double tho[S.tile_size];
        struct timeval start, end;

        unsigned int cpu, node;
        getcpu(&cpu, &node);
        if (TPM_TRACE)
          tpm_upstream_set_task_cpu_node(cpu, node, name_with_id_char);

        gettimeofday(&start, NULL);
        tpm_dtsqrt(A.tile_size, tileA, A.tile_size, tileB, A.tile_size, tileS,
                   S.tile_size, &tho[0], &work[0]);
        gettimeofday(&end, NULL);

        if (TPM_TRACE)
          tpm_upstream_get_task_time(start, end, name_with_id_char);
      }
      for (n = k + 1; n < A.matrix_size / A.tile_size; n++) {
        double *tileA = A(k, n);
        double *tileS = S(m, k);
        double *tileB = A(m, n);
        double *tileC = A(m, k);

        char *name_with_id_char = tpm_unique_task_identifier("tsmqr", k, m, n);
        if (TPM_TRACE)
          tpm_upstream_set_task_name(name_with_id_char);
#pragma omp task depend(inout                                                  \
                        : tileA [0:S.tile_size * S.tile_size],                 \
                          tileB [0:S.tile_size * S.tile_size])                 \
    depend(in                                                                  \
           : tileC [0:S.tile_size * S.tile_size],                              \
             tileS [0:A.tile_size * S.tile_size])
        {
          double work[S.tile_size * S.tile_size];
          struct timeval start, end;

          unsigned int cpu, node;
          getcpu(&cpu, &node);
          if (TPM_TRACE)
            tpm_upstream_set_task_cpu_node(cpu, node, name_with_id_char);

          gettimeofday(&start, NULL);
          tpm_dtsmqr(tpm_left, tpm_transpose, A.tile_size, A.tile_size,
                     A.tile_size, A.tile_size, tileA, A.tile_size, tileB,
                     A.tile_size, tileC, A.tile_size, tileS, S.tile_size,
                     &work[0], A.tile_size);
          gettimeofday(&end, NULL);

          if (TPM_TRACE)
            tpm_upstream_get_task_time(start, end, name_with_id_char);
        }
      }
    }
  }
}
