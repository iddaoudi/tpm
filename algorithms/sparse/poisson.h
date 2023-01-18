/*
 * =====================================================================================
 *
 *       Filename:  poisson.h
 *
 *    Description:  Task-based 2 dimensions poisson algorithm
 *
 *        Version:  1.0
 *        Created:  11/01/2023 19:12:34
 *       Revision:  none
 *       Compiler:  clang
 *
 *         Author:  Idriss Daoudi <idaoudi@anl.gov>
 *   Organization:  Argonne National Laboratory
 *
 * =====================================================================================
 */

#include <math.h>

/* Right hand side vector initialization */
void rhs(int matrix_size, double *f_, int tile_size) {
	double (*f)[matrix_size][matrix_size] = (double (*)[matrix_size][matrix_size])f_;
	double x, y;

	int i, j, k, l;
#pragma omp parallel
#pragma omp master
	for (j = 0; j < matrix_size; j += tile_size)
	{
		for (i = 0; i < matrix_size; i += tile_size)
		{
#pragma omp task firstprivate(tile_size, i, j, matrix_size) private(l, k, x, y) shared(f)
			for (k = j; k < j+tile_size; k++)
			{
				y = (double)(k) / (double)(matrix_size - 1);
				for (l = i; l < i+tile_size; l++)
				{
					x = (double)(l) / (double)(matrix_size - 1);
					if (l == 0 || l == nx - 1 || k == 0 || k == ny - 1)
						(*f)[l][k] = u_solution(x, y);
					else
						(*f)[l][k] = - u_solution_derivative(x, y);
				}
			}
		}
	}
}

double u_solution(double x, double y) {
    double pi = 3.141592653589793;
    return sin(pi * x * y);
}

double u_solution_derivative(double x, double y) {
    double pi = 3.141592653589793;
    return - pi * pi * (x * x + y * y) * sin(pi * x * y);
}

void poisson (int matrix_size, int tile_size) {
	double dx, dy; //FIXME dx = dy

	double *f_ = malloc(matrix_size * matrix_size * sizeof(double));
	double (*f)[matrix_size][matrix_size] = (double (*)[matrix_size][matrix_size])f_;

	double *u = malloc(matrix_size * matrix_size * sizeof(double));
	double *u_new_ = malloc(matrix_size * matrix_size * sizeof(double));
	double (*u_new)[matrix_size][matrix_size] = (double (*)[matrix_size][matrix_size]u_new_);

	dx = 1.0/(double)(matrix_size - 1);

	rhs(matrix_size, f_tmp, tile_size);

	int i, j, k, l;
#pragma omp parallel
#pragma omp master
	for (j = 0; j < matrix_size; j+=tile_size) {
		for (i = 0; i < matrix_size; i+=tile_size) {
#pragma omp task firstprivate(i, j) private(k, l)
			{
				for (k = j; k < j+tile_size; k++) {
					for (l = i; l < i+tile_size; l++) {
						if (l == 0 || l == nx - 1 || k == 0 || k == ny - 1) {
							(*u_new)[l][k] = (*f)[l][k];
						} else {
							(*u_new)[l][k] = 0.0;
						}
					}
				}
			}
		}
	}
	sweep();

}
