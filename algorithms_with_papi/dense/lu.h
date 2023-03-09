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
  for (k = 0; k < A.matrix_size / A.tile_size; k++) {
    double *tileA = A(k, k);

#pragma omp task depend(inout : tileA[0])
    {
      tpm_dgetrf(A.tile_size, tileA, A.tile_size);
    }
    for (m = k + 1; m < A.matrix_size / A.tile_size; m++) {
      double *tileA = A(k, k);
      double *tileB = A(m, k);

#pragma omp task depend(in                                                     \
                        : tileA [0:A.tile_size * A.tile_size])                 \
    depend(inout                                                               \
           : tileB[A.tile_size * A.tile_size])
      {
        cblas_dtrsm(CblasColMajor, CblasRight, CblasUpper, CblasNoTrans,
                    CblasNonUnit, A.tile_size, A.tile_size, 1.0, tileA,
                    A.tile_size, tileB, A.tile_size);
      }
    }
    for (n = k + 1; n < A.matrix_size / A.tile_size; n++) {
      double *tileA = A(k, k);
      double *tileB = A(k, n);

#pragma omp task depend(in                                                     \
                        : tileA [0:A.tile_size * A.tile_size])                 \
    depend(inout                                                               \
           : tileB[A.tile_size * A.tile_size])
      {
        cblas_dtrsm(CblasColMajor, CblasLeft, CblasLower, CblasNoTrans,
                    CblasUnit, A.tile_size, A.tile_size, 1.0, tileA,
                    A.tile_size, tileB, A.tile_size);
      }
      for (m = k + 1; m < A.matrix_size / A.tile_size; m++) {
        double *tileA = A(m, k);
        double *tileB = A(k, n);
        double *tileC = A(m, n);

#pragma omp task depend(in                                                     \
                        : tileA [0:A.tile_size * A.tile_size],                 \
                          tileB [0:A.tile_size * A.tile_size])                 \
    depend(inout                                                               \
           : tileC [0:A.tile_size * A.tile_size])
        {
          cblas_dgemm(CblasColMajor, CblasNoTrans, CblasNoTrans, A.tile_size,
                      A.tile_size, A.tile_size, -1.0, tileA, A.tile_size, tileB,
                      A.tile_size, 1.0, tileC, A.tile_size);
        }
      }
    }
  }
}
