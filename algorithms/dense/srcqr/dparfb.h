/*
 * =====================================================================================
 *
 *       Filename:  dparfb.h
 *
 *    Description:  DPARFB implementation
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
#include <cblas.h>
#include <lapacke.h>

#include "dpamm.h"

int tpm_dparfb(int side, int transpose, int direct, int store_column_row, int M,
               int N, int K, int L, double *tileA1, int lda1, double *tileA2,
               int lda2, const double *tileS, int lds, const double *tileB,
               int ldb, double *workspace, int ldw) {
  if (M == 0 || N == 0 || K == 0)
    return 0;

  if (direct == tpm_forward) {
    if (side == tpm_left) {
      tpm_dpamm(tpm_W, tpm_left, store_column_row, K, M, N, L, tileA1, lda1,
                tileA2, lda2, tileS, lds, workspace, ldw);
      cblas_dtrmm(CblasColMajor, CblasLeft, CblasUpper, transpose, CblasNonUnit,
                  K, N, 1.0, tileB, ldb, workspace, ldw);
      for (int j = 0; j < M; j++) {
        cblas_daxpy(K, -1.0, &workspace[ldw * j], 1, &tileA1[lda1 * j], 1);
      }
      tpm_dpamm(tpm_A2, tpm_left, store_column_row, N, N, K, L, tileA1, lda1,
                tileA2, lda2, tileS, lds, workspace, ldw);
    } else {
      tpm_dpamm(tpm_W, tpm_right, store_column_row, M, K, N, L, tileA1, lda1,
                tileA2, lda2, tileS, lds, workspace, ldw);
      cblas_dtrmm(CblasColMajor, CblasRight, CblasUpper, transpose,
                  CblasNonUnit, N, K, 1.0, tileB, ldb, workspace, ldw);
      for (int j = 0; j < K; j++) {
        cblas_daxpy(M, -1.0, &workspace[ldw * j], 1, &tileA1[lda1 * j], 1);
      }
      tpm_dpamm(tpm_A2, tpm_right, store_column_row, N, N, K, L, tileA1, lda1,
                tileA2, lda2, tileS, lds, workspace, ldw);
    }
  } else {
    return 1;
  }
  return 0;
}
