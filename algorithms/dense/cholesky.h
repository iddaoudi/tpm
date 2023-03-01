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
  int k = 0, m = 0, n = 0;
  for (k = 0; k < A.matrix_size / A.tile_size; k++) {
    double *tileA = A(k, k);

#pragma omp task                               \
    depend(inout                                                               \
           : tileA [0:A.tile_size * A.tile_size])
    {
      LAPACKE_dpotrf(LAPACK_COL_MAJOR, 'U', A.tile_size, tileA, A.tile_size);
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
}
