/*
 * =====================================================================================
 *
 *       Filename:  dgeqrt.h
 *
 *    Description:  DGEQRT implementation
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

int tpm_dgeqrt(int tileA_size, int Stile_size, double *tileA, int lda,
               double *tileS, int lds, double *tho, double *workspace) {
  int gamma;
  if (tileA_size == 0 || Stile_size == 0)
    return 0;
  int k = tileA_size;
  for (int i = 0; i < k; i += Stile_size) {
    gamma = min(Stile_size, k - i);
    LAPACKE_dgeqr2_work(LAPACK_COL_MAJOR, tileA_size - i, gamma,
                        &tileA[lda * i + i], lda, &tho[i], workspace);
    LAPACKE_dlarft_work(LAPACK_COL_MAJOR, 'F', 'C', tileA_size - i, gamma,
                        &tileA[lda * i + i], lda, &tho[i], &tileS[lds * i],
                        lds);
    if (tileA_size > i + gamma) {
      LAPACKE_dlarfb_work(LAPACK_COL_MAJOR, 'L', 'T', 'F', 'C', tileA_size - i,
                          tileA_size - i - gamma, gamma, &tileA[lda * i + i],
                          lda, &tileS[lds * i], lds,
                          &tileA[lda * (i + gamma) + i], lda, workspace,
                          tileA_size - i - gamma);
    }
  }
  return 0;
}
