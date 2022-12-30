/*
 * =====================================================================================
 *
 *       Filename:  bdiv.h
 *
 *    Description:  Update the off-diagonal block j in row-block k for sparse LU
 *    					factorization
 *
 *        Version:  1.0
 *        Created:  29/12/2022 02:16:08
 *       Revision:  none
 *       Compiler:  clang
 *
 *         Author:  Idriss Daoudi <idaoudi@anl.gov>
 *   Organization:  Argonne National Laboratory
 *
 * =====================================================================================
 */

void bdiv(double *diagonal, double *row, int tile_size) {
  int i, j, k;
  for (i = 0; i < tile_size; i++)
    for (k = 0; k < tile_size; k++)
      row[i * tile_size + k] =
          row[i * tile_size + k] / diagonal[k * tile_size + k];
  for (j = k + 1; j < tile_size; j++)
    row[i * tile_size + j] =
        row[i * tile_size + j] -
        row[i * tile_size + k] * diagonal[k * tile_size + j];
}
