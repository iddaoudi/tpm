/*
 * =====================================================================================
 *
 *       Filename:  lu0.h
 *
 *    Description:  LU factorization for sparse matrix
 *
 *        Version:  1.0
 *        Created:  29/12/2022 01:00:06
 *       Revision:  none
 *       Compiler:  clang
 *
 *         Author:  Idriss Daoudi <idaoudi@anl.gov> 
 *   Organization:  Argonne National Laboratory
 *
 * =====================================================================================
 */

void lu0(double *diagonal, int tile_size) {
	int i = 0, j = 0, k = 0;

	for (k = 0; k < tile_size; k++)
	{
		for (i = k + 1; i < tile_size; i++)
		{
			//printf("lu0 %d %d\n", i, k);
			diagonal[i * tile_size + k] = diagonal[i * tile_size + k] / diagonal[k * tile_size + k];
			//printf("diagonal[i*tile + k] = %f\n", diagonal[i*tile_size+k]);
			for (j = k + 1; j < tile_size; j++){
				diagonal[i * tile_size + j] = diagonal[i * tile_size + j] - diagonal[i * tile_size + k] * diagonal[k * tile_size + j];
			}
		}
	}
}
