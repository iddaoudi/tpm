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

void sparselu(double **M, int matrix_size, int tile_size) {
  
  int i = 0, j = 0, k = 0;
#pragma omp parallel private(i, j, k) shared(M)
#pragma omp single
  {
    for (k = 0; k < matrix_size; k++) {
#pragma omp task firstprivate(k) shared(M)                                     \
    depend(inout                                                               \
           : M [k * matrix_size + k:tile_size * tile_size])
      lu0(M[k * matrix_size + k], tile_size);
	 	printf("lu0 %d - %d - %d\n", i, j, k);
      for (j = k + 1; j < matrix_size; j++) {
        if (M[k * matrix_size + j] != NULL) {
#pragma omp task firstprivate(k, j) shared(M)                                  \
    depend(in                                                                  \
           : M [k * matrix_size + k:tile_size * tile_size])              \
        depend(inout                                                           \
               : M [k * matrix_size + j:tile_size * tile_size])
          fwd(M[k * matrix_size + k], M[k * matrix_size + j],
              tile_size);
	 		 printf("fwd %d - %d - %d\n", i, j, k);
        }
      }
      for (i = k + 1; i < matrix_size; i++) {
        if (M[i * matrix_size + k] != NULL) {
#pragma omp task firstprivate(k, i) shared(M)                                  \
    depend(in                                                                  \
           : M [k * matrix_size + k:tile_size * tile_size])              \
        depend(inout                                                           \
               : M [i * matrix_size + k:tile_size * tile_size])
          bdiv(M[k * matrix_size + k], M[i * matrix_size + k],
               tile_size);
	 		 printf("bdiv %d - %d - %d\n", i, j, k);
        }
      }
      for (i = k + 1; i < matrix_size; i++) {
        if (M[i * matrix_size + k] != NULL) {
          for (j = k + 1; j < matrix_size; j++) {
            if (M[k * matrix_size + j] != NULL) {
              if (M[i * matrix_size + j] == NULL) {
                M[i * matrix_size + j] =
                    tpm_allocate_empty_block(tile_size);
              }
#pragma omp task firstprivate(i, j, k) shared(M)                               \
    depend(in                                                                  \
           : M [i * matrix_size + k:tile_size * tile_size],              \
             M [k * matrix_size + j:tile_size * tile_size])              \
        depend(inout                                                           \
               : M [i * matrix_size + j:tile_size * tile_size])
              bmod(M[i * matrix_size + k], M[k * matrix_size + j],
                   M[i * matrix_size + j], tile_size);
	 		 	  printf("bmod %d - %d - %d\n", i, j, k);
            }
          }
        }
      }
    }
#pragma omp taskwait
  }
}
