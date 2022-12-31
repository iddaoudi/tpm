/*
 * =====================================================================================
 *
 *       Filename:  sparselu.h
 *
 *    Description:  Task-based sparse LU algorithm
 *
 *        Version:  1.0
 *        Created:  28/12/2022 19:02:18
 *       Revision:  none
 *       Compiler:  clang
 *
 *         Author:  Idriss Daoudi <idaoudi@anl.gov>
 *   Organization:  Argonne National Laboratory
 *
 * =====================================================================================
 */

#include <sys/time.h>

void sparselu(double **M, int matrix_size, int tile_size) {
  int i, j, k;
  struct timeval start, end;
  unsigned int cpu, node;
  if (TPM_TRACE)
    tpm_downstream_start("sparselu", matrix_size, tile_size, NTH);
  char *name_with_id_char = tpm_unique_task_identifier("lu0", i, j, k);
  if (TPM_TRACE) tpm_upstream_set_task_name(name_with_id_char);
#pragma omp parallel private(i, j, k) shared(M)
#pragma omp single /* nowait */
  {
    for (k = 0; k < matrix_size; k++) {
#pragma omp task firstprivate(k) shared(M)                                     \
    depend(inout                                                               \
           : M [k * matrix_size + k:tile_size * tile_size])
      {
          char *name_with_id_char = tpm_unique_task_identifier("lu0", i, j, k);
          if (TPM_TRACE) tpm_upstream_set_task_name(name_with_id_char);
          cpu = 0;
          node = 0;
          start = (struct timeval){0};
          end = (struct timeval){0};
          if (TPM_TRACE) getcpu(&cpu, &node);
          if (TPM_TRACE) tpm_upstream_set_task_cpu_node(cpu, node,
          name_with_id_char); if (TPM_TRACE) gettimeofday(&start, NULL); 
          lu0(M[k * matrix_size + k], tile_size);
          if (TPM_TRACE) gettimeofday(&end, NULL);
          if (TPM_TRACE) tpm_upstream_get_task_time(start, end, name_with_id_char);
      }
      for (j = k + 1; j < matrix_size; j++)
        if (M[k * matrix_size + j] != NULL) {
          char *name_with_id_char = tpm_unique_task_identifier("fwd", i, j, k);
          if (TPM_TRACE) tpm_upstream_set_task_name(name_with_id_char);
#pragma omp task firstprivate(j, k) shared(M)                                  \
    depend(in                                                                  \
           : M [k * matrix_size + k:tile_size * tile_size])                    \
        depend(inout                                                           \
               : M [k * matrix_size + j:tile_size * tile_size])
          {
            cpu = 0;
            node = 0;
            start = (struct timeval){0};
            end = (struct timeval){0};
            if (TPM_TRACE) getcpu(&cpu, &node);
            if (TPM_TRACE) tpm_upstream_set_task_cpu_node(cpu, node, name_with_id_char);
            if (TPM_TRACE) gettimeofday(&start, NULL); 
            fwd(M[k * matrix_size + k], M[k * matrix_size + j], tile_size);
            if (TPM_TRACE) gettimeofday(&end, NULL);
            if (TPM_TRACE) tpm_upstream_get_task_time(start, end, name_with_id_char);
          }
        }
      for (i = k + 1; i < matrix_size; i++)
        if (M[i * matrix_size + k] != NULL) {
          char *name_with_id_char = tpm_unique_task_identifier("bdiv", i, j, k);
          if (TPM_TRACE) tpm_upstream_set_task_name(name_with_id_char);
#pragma omp task firstprivate(i, k) shared(M)                                  \
    depend(in                                                                  \
           : M [k * matrix_size + k:tile_size * tile_size])                    \
        depend(inout                                                           \
               : M [i * matrix_size + k:tile_size * tile_size])
          {
            cpu = 0;
            node = 0;
            start = (struct timeval){0};
            end = (struct timeval){0};
            if (TPM_TRACE) getcpu(&cpu, &node);
            if (TPM_TRACE) tpm_upstream_set_task_cpu_node(cpu, node, name_with_id_char);
            if (TPM_TRACE) gettimeofday(&start, NULL);
            bdiv(M[k * matrix_size + k], M[i * matrix_size + k], tile_size); 
            if (TPM_TRACE) gettimeofday(&end, NULL);
            if (TPM_TRACE) tpm_upstream_get_task_time(start, end, name_with_id_char);
          }
        }

      for (i = k + 1; i < matrix_size; i++)
        if (M[i * matrix_size + k] != NULL)
          for (j = k + 1; j < matrix_size; j++)
            if (M[k * matrix_size + j] != NULL) {
              if (M[i * matrix_size + j] == NULL)
                M[i * matrix_size + j] = tpm_allocate_empty_block(tile_size);
              char *name_with_id_char = tpm_unique_task_identifier("bmod", i, j, k);
              if (TPM_TRACE) tpm_upstream_set_task_name(name_with_id_char);
#pragma omp task firstprivate(k, j, i) shared(M)                               \
    depend(in                                                                  \
           : M [i * matrix_size + k:tile_size * tile_size],                    \
             M [k * matrix_size + j:tile_size * tile_size])                    \
        depend(inout                                                           \
               : M [i * matrix_size + j:tile_size * tile_size])
              {
                cpu = 0;
                node = 0;
                start = (struct timeval){0};
                end = (struct timeval){0};
                if (TPM_TRACE) getcpu(&cpu, &node);
                if (TPM_TRACE) tpm_upstream_set_task_cpu_node(cpu, node, name_with_id_char);
                if (TPM_TRACE) gettimeofday(&start, NULL);
                bmod(M[i * matrix_size + k], M[k * matrix_size + j],
                     M[i * matrix_size + j], tile_size);
                if (TPM_TRACE) gettimeofday(&end, NULL);
                if (TPM_TRACE) tpm_upstream_get_task_time(start, end, name_with_id_char);
              }
            }
    }
#pragma omp taskwait
  }
}
