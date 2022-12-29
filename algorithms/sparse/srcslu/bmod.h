/*
 * =====================================================================================
 *
 *       Filename:  bmod.h
 *
 *    Description:  Compute the contribution of block i in row-block k for block
 *    					k in column-block j for sparse LU factorization
 *
 *        Version:  1.0
 *        Created:  29/12/2022 01:43:15
 *       Revision:  none
 *       Compiler:  clang
 *
 *         Author:  Idriss Daoudi <idaoudi@anl.gov> 
 *   Organization:  Argonne National Laboratory
 *
 * =====================================================================================
 */

void bmod(double *row, double *column, double *inner_tile, int tile_size)
{
	int i = 0, j = 0, k = 0;
	for (i = 0; i < tile_size; i++) {
		for (j = 0; j < tile_size; j++) {
			for (k = 0; k < tile_size; k++) {
				inner_tile[i * tile_size + j] = inner_tile[i * tile_size + j] - row[i * tile_size + k] * column[k * tile_size + j];
			}
		}
	}
}
