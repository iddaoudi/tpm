/*
 * =====================================================================================
 *
 *       Filename:  print.h
 *
 *    Description:  Useful print functions
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

#include <stdio.h>

void tpm_print_matrix(tpm_desc A) {
  double *B = malloc(A.matrix_size * A.matrix_size * sizeof(double));
  int matrix_counter = 0;
  int tile_counter = 0;

  for (int i = 0; i < A.matrix_size / A.tile_size; i++) {
    for (int j = 0; j < A.matrix_size / A.tile_size; j++) {
      double *dA = A(i, j);
      for (int k = 0; k < A.tile_size; k++) {
        for (int l = 0; l < A.tile_size; l++) {
          B[k * A.matrix_size + l + A.tile_size * j * A.matrix_size +
            i * A.tile_size] = dA[k * A.tile_size + l];
        }
      }
      tile_counter += A.tile_size;
    }
    matrix_counter += A.matrix_size;
  }
  for (int i = 0; i < A.matrix_size * A.matrix_size; i++) {
    printf("%f ", B[i]);
    if ((i + 1) % A.matrix_size == 0)
      printf("\n");
  }
}

void tpm_simple_print_matrix(tpm_desc A) {
  int counter = 0;
  for (int i = 0; i < A.matrix_size / A.tile_size; i++) {
    for (int j = 0; j < A.matrix_size / A.tile_size; j++) {
      printf("i, j: %d, %d\n", i, j);
      double *dA = A(i, j);
      for (int k = 0; k < A.tile_size; k++) {
        for (int l = 0; l < A.tile_size; l++) {
          printf("%f ", dA[k * A.tile_size + l]);
        }
        printf("\n");
      }
      printf("\n");
      counter++;
    }
  }
}
