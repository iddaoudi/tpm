/*
 * =====================================================================================
 *
 *       Filename:  fwd.h
 *
 *    Description:  Factorize column-block k and compute all the contributions for
 *    					sparse LU factorization
 *
 *        Version:  1.0
 *        Created:  29/12/2022 01:24:12
 *       Revision:  none
 *       Compiler:  clang
 *
 *         Author:  Idriss Daoudi <idaoudi@anl.gov>
 *   Organization:  Argonne National Laboratory
 *
 * =====================================================================================
 */

void fwd(double *diagonal, double *column, int tile_size) 
{
	int i = 0, j = 0, k = 0;
	for (j = 0; j < tile_size; j++) {
		for (k = 0; k < tile_size; k++) {
			for (i = k + 1; i < tile_size; i++) {
				//printf("%d - %d - %d - %d\n", i * tile_size + j, i * tile_size + j, i * tile_size + k, k * tile_size + j);
				column[i * tile_size + j] = column[i * tile_size + j] - diagonal[i * tile_size + k] * column[k * tile_size + j];
				//printf("done fwd\n");
			}
		}
	}
}
