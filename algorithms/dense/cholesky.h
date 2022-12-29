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

void cholesky(tpm_desc A) {
  if (TPM_TRACE)
    tpm_downstream_start("cholesky", A.matrix_size, A.tile_size, NTH);
  int k = 0, m = 0, n = 0;
  for (k = 0; k < A.matrix_size / A.tile_size; k++) {
    double *tileA = A(k, k);

    char *name_with_id_char = tpm_unique_task_identifier("potrf", k, m, n);
    if (TPM_TRACE)
      tpm_upstream_set_task_name(name_with_id_char);
#pragma omp task firstprivate(name_with_id_char)                               \
    depend(inout                                                               \
           : tileA [0:A.tile_size * A.tile_size])
    {
      struct timeval start, end;

      unsigned int cpu, node;
      getcpu(&cpu, &node);
      if (TPM_TRACE)
        tpm_upstream_set_task_cpu_node(cpu, node, name_with_id_char);

      gettimeofday(&start, NULL);
      LAPACKE_dpotrf(LAPACK_COL_MAJOR, 'U', A.tile_size, tileA, A.tile_size);
      gettimeofday(&end, NULL);

      if (TPM_TRACE)
        tpm_upstream_get_task_time(start, end, name_with_id_char);
    }
	 for (m = k + 1; m < A.matrix_size / A.tile_size; m++) {
      double *tileA = A(k, k);
      double *tileB = A(k, m);

      char *name_with_id_char = tpm_unique_task_identifier("trsm", k, m, n);
      if (TPM_TRACE)
        tpm_upstream_set_task_name(name_with_id_char);
#pragma omp task firstprivate(name_with_id_char)                               \
    depend(in                                                                  \
           : tileA [0:A.tile_size * A.tile_size])                              \
        depend(inout                                                           \
               : tileB [0:A.tile_size * A.tile_size])
      {
        struct timeval start, end;

        unsigned int cpu, node;
        getcpu(&cpu, &node);
        if (TPM_TRACE)
          tpm_upstream_set_task_cpu_node(cpu, node, name_with_id_char);

        gettimeofday(&start, NULL);
        cblas_dtrsm(CblasColMajor, CblasLeft, CblasUpper, CblasTrans,
                    CblasNonUnit, A.tile_size, A.tile_size, 1.0, tileA,
                    A.tile_size, tileB, A.tile_size);
        gettimeofday(&end, NULL);

        if (TPM_TRACE)
          tpm_upstream_get_task_time(start, end, name_with_id_char);
      }
    }

    for (m = k + 1; m < A.matrix_size / A.tile_size; m++) {
      double *tileA = A(k, m);
      double *tileB = A(m, m);

      char *name_with_id_char = tpm_unique_task_identifier("syrk", k, m, n);
      if (TPM_TRACE)
        tpm_upstream_set_task_name(name_with_id_char);
#pragma omp task firstprivate(name_with_id_char)                               \
    depend(in                                                                  \
           : tileA [0:A.tile_size * A.tile_size])                              \
        depend(inout                                                           \
               : tileB [0:A.tile_size * A.tile_size])
      {
        struct timeval start, end;

        unsigned int cpu, node;
        getcpu(&cpu, &node);
        if (TPM_TRACE)
          tpm_upstream_set_task_cpu_node(cpu, node, name_with_id_char);

        gettimeofday(&start, NULL);
        cblas_dsyrk(CblasColMajor, CblasUpper, CblasTrans, A.tile_size,
                    A.tile_size, -1.0, tileA, A.tile_size, 1.0, tileB,
                    A.tile_size);
        gettimeofday(&end, NULL);

        if (TPM_TRACE)
          tpm_upstream_get_task_time(start, end, name_with_id_char);
      }
      
		for (n = k + 1; n < m; n++) {
        double *tileA = A(k, n);
        double *tileB = A(k, m);
        double *tileC = A(n, m);

        char *name_with_id_char = tpm_unique_task_identifier("gemm", k, m, n);
        if (TPM_TRACE)
          tpm_upstream_set_task_name(name_with_id_char);
#pragma omp task firstprivate(name_with_id_char)                               \
    depend(in                                                                  \
           : tileA [0:A.tile_size * A.tile_size],                              \
             tileB [0:A.tile_size * A.tile_size])                              \
        depend(inout                                                           \
               : tileC [0:A.tile_size * A.tile_size])
        {
          struct timeval start, end;

          unsigned int cpu, node;
          getcpu(&cpu, &node);
          if (TPM_TRACE)
            tpm_upstream_set_task_cpu_node(cpu, node, name_with_id_char);

          gettimeofday(&start, NULL);
          cblas_dgemm(CblasColMajor, CblasTrans, CblasNoTrans, A.tile_size,
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
