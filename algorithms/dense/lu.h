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
  if (TPM_TRACE)
    tpm_downstream_start("lu", A.matrix_size, A.tile_size, NTH);
  int k = 0, m = 0, n = 0;
  for (k = 0; k < A.matrix_size / A.tile_size; k++) {
    double *tileA = A(k, k);

	 char *name_with_id_char = tpm_unique_task_identifier("getrf", k, m, n);
    if (TPM_TRACE)
      tpm_upstream_set_task_name(name_with_id_char);
#pragma omp task depend(inout : tileA[0])
    {
      struct timeval start, end;

      unsigned int cpu, node;
      getcpu(&cpu, &node);
      if (TPM_TRACE)
        tpm_upstream_set_task_cpu_node(cpu, node, name_with_id_char);

      gettimeofday(&start, NULL);
      tpm_dgetrf(A.tile_size, tileA, A.tile_size);
      gettimeofday(&end, NULL);

      if (TPM_TRACE)
        tpm_upstream_get_task_time(start, end, name_with_id_char);
    }
    for (m = k + 1; m < A.matrix_size / A.tile_size; m++) {
      double *tileA = A(k, k);
      double *tileB = A(m, k);

      char *name_with_id_char = tpm_unique_task_identifier("trsm", k, m, n);
      if (TPM_TRACE)
        tpm_upstream_set_task_name(name_with_id_char);
#pragma omp task depend(in                                                     \
                        : tileA [0:A.tile_size * A.tile_size])                 \
    depend(inout                                                               \
           : tileB[A.tile_size * A.tile_size])
      {
        struct timeval start, end;

        unsigned int cpu, node;
        getcpu(&cpu, &node);
        if (TPM_TRACE)
          tpm_upstream_set_task_cpu_node(cpu, node, name_with_id_char);

        gettimeofday(&start, NULL);
        cblas_dtrsm(CblasColMajor, CblasRight, CblasUpper, CblasNoTrans,
                    CblasNonUnit, A.tile_size, A.tile_size, 1.0, tileA,
                    A.tile_size, tileB, A.tile_size);
        gettimeofday(&end, NULL);

        if (TPM_TRACE)
          tpm_upstream_get_task_time(start, end, name_with_id_char);
      }
    }
    for (n = k + 1; n < A.matrix_size / A.tile_size; n++) {
      double *tileA = A(k, k);
      double *tileB = A(k, n);

      char *name_with_id_char = tpm_unique_task_identifier("trsm", k, m, n);
      if (TPM_TRACE)
        tpm_upstream_set_task_name(name_with_id_char);
#pragma omp task depend(in                                                     \
                        : tileA [0:A.tile_size * A.tile_size])                 \
    depend(inout                                                               \
           : tileB[A.tile_size * A.tile_size])
      {
        struct timeval start, end;

        unsigned int cpu, node;
        getcpu(&cpu, &node);
        if (TPM_TRACE)
          tpm_upstream_set_task_cpu_node(cpu, node, name_with_id_char);

        gettimeofday(&start, NULL);
        cblas_dtrsm(CblasColMajor, CblasLeft, CblasLower, CblasNoTrans,
                    CblasUnit, A.tile_size, A.tile_size, 1.0, tileA,
                    A.tile_size, tileB, A.tile_size);
        gettimeofday(&end, NULL);

        if (TPM_TRACE)
          tpm_upstream_get_task_time(start, end, name_with_id_char);
      }
      for (m = k + 1; m < A.matrix_size / A.tile_size; m++) {
        double *tileA = A(m, k);
        double *tileB = A(k, n);
        double *tileC = A(m, n);

        char *name_with_id_char = tpm_unique_task_identifier("gemm", k, m, n);
        if (TPM_TRACE)
          tpm_upstream_set_task_name(name_with_id_char);
#pragma omp task depend(in                                                     \
                        : tileA [0:A.tile_size * A.tile_size],                 \
                          tileB [0:A.tile_size * A.tile_size])                 \
    depend(inout                                                               \
           : tileC [0:A.tile_size * A.tile_size])
        {
          struct timeval start, end;

          unsigned int cpu, node;
          getcpu(&cpu, &node);
          if (TPM_TRACE)
            tpm_upstream_set_task_cpu_node(cpu, node, name_with_id_char);

          gettimeofday(&start, NULL);
          cblas_dgemm(CblasColMajor, CblasNoTrans, CblasNoTrans, A.tile_size,
                      A.tile_size, A.tile_size, -1.0, tileA, A.tile_size, tileB,
                      A.tile_size, 1.0, tileC, A.tile_size);
          gettimeofday(&end, NULL);

          if (TPM_TRACE)
            tpm_upstream_get_task_time(start, end, name_with_id_char);
        }
      }
    }
  }
}
