/*
 * =====================================================================================
 *
 *       Filename:  dtsqrt.h
 *
 *    Description:  DTSQRT implementation
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

int tpm_dtsqrt(int tileA_size, double *tileA1, int lda1, double *tileA2,
               int lda2, double *tileS, int lds, double *tho,
               double *workspace) {
  double alpha;
  int lambda, gamma;
  if (tileA_size == 0)
    return 0;

  for (lambda = 0; lambda < tileA_size; lambda += tileA_size) {
    gamma = min(tileA_size - lambda, tileA_size);
    for (int i = 0; i < gamma; i++) {
      LAPACKE_dlarfg_work(tileA_size + 1,
                          &tileA1[lda1 * (lambda + i) + lambda + i],
                          &tileA2[lda2 * (lambda + i)], 1, &tho[lambda + i]);
      if (lambda + i + 1 < tileA_size) {
        alpha = -tho[lambda + i];
        cblas_dcopy(gamma - i - 1,
                    &tileA1[lda1 * (lambda + i + 1) + (lambda + i)], lda1,
                    workspace, 1);
        cblas_dgemv(CblasColMajor, CblasTrans, tileA_size, gamma - i - 1, 1.0,
                    &tileA2[lda2 * (lambda + i + 1)], lda2,
                    &tileA2[lda2 * (lambda + i)], 1, 1.0, workspace, 1);
        cblas_daxpy(gamma - i - 1, alpha, workspace, 1,
                    &tileA1[lda1 * (lambda + i + 1) + lambda + i], lda1);
        cblas_dger(CblasColMajor, tileA_size, gamma - i - 1, alpha,
                   &tileA2[lda2 * (lambda + i)], 1, workspace, 1,
                   &tileA2[lda2 * (lambda + i + 1)], lda2);
      }
      alpha = -tho[lambda + i];
      cblas_dgemv(CblasColMajor, CblasTrans, tileA_size, i, alpha,
                  &tileA2[lda2 * lambda], lda2, &tileA2[lda2 * (lambda + i)], 1,
                  0.0, &tileS[lds * (lambda + i)], 1);
      cblas_dtrmv(CblasColMajor, CblasUpper, CblasNoTrans, CblasNonUnit, i,
                  &tileS[lds * lambda], lds, &tileS[lds * (lambda + i)], 1);
      tileS[lds * (lambda + i) + i] = tho[lambda + i];
    }
    if (tileA_size > lambda + gamma) {
      tpm_dtsmqr(tpm_left, tpm_transpose, gamma, tileA_size - (lambda + gamma),
                 tileA_size, tileA_size,
                 &tileA1[lda1 * (lambda + gamma) + lambda], lda1,
                 &tileA2[lda2 * (lambda + gamma)], lda2, &tileA2[lda2 * lambda],
                 lda2, &tileS[lds * lambda], lds, workspace, gamma);
    }
  }
  return 0;
}
