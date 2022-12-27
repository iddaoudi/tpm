/*
 * =====================================================================================
 *
 *       Filename:  dtsmqr.h
 *
 *    Description:  DTSMQR implementation
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

#include "dparfb.h"

int tpm_dtsmqr(int side, int transpose, int M, int N, int K, int B,
               double *tileA1, int lda1, double *tileA2, int lda2,
               const double *tileS, int lds, const double *tileB, int ldb,
               double *workspace, int ldw) {
  if (M == 0 || N == 0 || K == 0 || B == 0)
    return 0;

  int i1, i3, gamma;
  int ic = 0;
  int m = M;
  if ((side == tpm_left && transpose != tpm_notranspose) ||
      (side == tpm_right && transpose == tpm_notranspose)) {
    i1 = 0;
    i3 = B;
  } else {
    i1 = K - 1;
    i3 = -B;
  }

  for (int i = i1; i > -1 && i < K; i += i3) {
    gamma = min(B, K - i);
    if (side == tpm_left) {
      m = M - i;
      ic = i;
    }
    tpm_dparfb(side, transpose, tpm_forward, tpm_column, m, N, gamma, 0,
               &tileA1[lda1 * ic + ic], lda1, tileA2, lda2, &tileS[lds * i],
               lds, &tileB[ldb * i], ldb, workspace, ldw);
  }
  return 0;
}
