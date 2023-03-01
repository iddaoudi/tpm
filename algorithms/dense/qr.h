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
  for (k = 0; k < A.matrix_size / A.tile_size; k++) {
    double *tileA = A(k, k);
    double *tileS = S(k, k);

#pragma omp task depend(inout                                                  \
                        : tileA [0:S.tile_size * S.tile_size])                 \
    depend(out                                                                 \
           : tileS [0:A.tile_size * S.tile_size])
    {
      double tho[S.tile_size];
      double work[S.tile_size * S.tile_size];
      tpm_dgeqrt(A.tile_size, S.tile_size, tileA, A.tile_size, tileS,
                 S.tile_size, &tho[0], &work[0]);
    }
    for (n = k + 1; n < A.matrix_size / A.tile_size; n++) {
      double *tileA = A(k, k);
      double *tileS = S(k, k);
      double *tileB = A(k, n);

#pragma omp task depend(in                                                     \
                        : tileA [0:S.tile_size * S.tile_size],                 \
                          tileS [0:A.tile_size * S.tile_size])                 \
    depend(inout                                                               \
           : tileB [0:S.tile_size * S.tile_size])
      {
        double work[S.tile_size * S.tile_size];
        tpm_dormqr(tpm_left, tpm_transpose, A.tile_size, tileA, A.tile_size,
                   tileS, S.tile_size, tileB, A.tile_size, &work[0],
                   S.tile_size);
      }
    }
    for (m = k + 1; m < A.matrix_size / A.tile_size; m++) {
      double *tileA = A(k, k);
      double *tileS = S(m, k);
      double *tileB = A(m, k);

#pragma omp task depend(inout                                                  \
                        : tileA [0:S.tile_size * S.tile_size],                 \
                          tileB [0:S.tile_size * S.tile_size])                 \
    depend(out                                                                 \
           : tileS [0:S.tile_size * A.tile_size])
      {
        double work[S.tile_size * S.tile_size];
        double tho[S.tile_size];
        tpm_dtsqrt(A.tile_size, tileA, A.tile_size, tileB, A.tile_size, tileS,
                   S.tile_size, &tho[0], &work[0]);
      }
      for (n = k + 1; n < A.matrix_size / A.tile_size; n++) {
        double *tileA = A(k, n);
        double *tileS = S(m, k);
        double *tileB = A(m, n);
        double *tileC = A(m, k);

#pragma omp task depend(inout                                                  \
                        : tileA [0:S.tile_size * S.tile_size],                 \
                          tileB [0:S.tile_size * S.tile_size])                 \
    depend(in                                                                  \
           : tileC [0:S.tile_size * S.tile_size],                              \
             tileS [0:A.tile_size * S.tile_size])
        {
          double work[S.tile_size * S.tile_size];
          tpm_dtsmqr(tpm_left, tpm_transpose, A.tile_size, A.tile_size,
                     A.tile_size, A.tile_size, tileA, A.tile_size, tileB,
                     A.tile_size, tileC, A.tile_size, tileS, S.tile_size,
                     &work[0], A.tile_size);
        }
      }
    }
  }
}
