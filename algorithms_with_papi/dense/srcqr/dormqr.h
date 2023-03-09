/*
 * =====================================================================================
 *
 *       Filename:  dormqr.h
 *
 *    Description:  DORMQR implementation
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

int tpm_dormqr(int side, int transpose, int tileA_size, const double *tileA,
               int lda, const double *tileS, int lds, double *tileB, int ldb,
               double *workspace, int ldw) {
  int gamma, i1, i3;
  int aindex = tileA_size;
  int cindex = 0;

  if (tileA_size == 0) {
    return 0;
  }
  if ((side == tpm_left && transpose != tpm_notranspose) ||
      (side == tpm_right && transpose == tpm_notranspose)) {
    i1 = 0;
    i3 = tileA_size;
  } else {
    i1 = tileA_size - 1;
    i3 = -tileA_size;
  }
  for (int i = i1; i > -1 && i < tileA_size; i += i3) {
    gamma = min(tileA_size, tileA_size - i);
    aindex = tileA_size - i;
    cindex = i;
    LAPACKE_dlarfb_work(LAPACK_COL_MAJOR, 'L', 'T', 'F', 'C', aindex, aindex,
                        gamma, &tileA[lda * i + i], lda, &tileS[lds * i], lds,
                        &tileB[ldb * cindex + cindex], ldb, workspace, ldw);
  }
  return 0;
}
