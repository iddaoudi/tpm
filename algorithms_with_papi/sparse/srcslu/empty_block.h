/*
 * =====================================================================================
 *
 *       Filename:  empty_block.h
 *
 *    Description:  Allocate empty block for bmod task in sparse LU
 * factorization
 *
 *        Version:  1.0
 *        Created:  29/12/2022 01:35:18
 *       Revision:  none
 *       Compiler:  clang
 *
 *         Author:  Idriss Daoudi <idaoudi@anl.gov>
 *   Organization:  Argonne National Laboratory
 *
 * =====================================================================================
 */

double *tpm_allocate_empty_block(int tile_size) {
  int i, j;
  double *p, *q;

  p = (double *)malloc(tile_size * tile_size * sizeof(double));
  q = p;
  if (p != NULL) {
    for (i = 0; i < tile_size; i++) {
      for (j = 0; j < tile_size; j++) {
        (*p) = 0.0;
        p++;
      }
    }
  } else {
    printf("Not enough memory\n");
    exit(1);
  }
  return (q);
}
