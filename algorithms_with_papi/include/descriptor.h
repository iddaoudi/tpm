/*
 * =====================================================================================
 *
 *       Filename:  descriptor.h
 *
 *    Description:  Matrix descriptor implementation
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
#include <stdlib.h>

/* Definition of descriptor structure */
typedef struct tpm_descriptor_t {
  int tile_size;
  long int matrix_nelements;
  long int tile_nelements;
  int matrix_size;
  void *matrix;
} tpm_desc;

/* Matrix descriptor initialization */
tpm_desc tpm_matrix_desc_init(int tile_size, long int matrix_nelements,
                              long int tile_nelements, int matrix_size) {
  tpm_desc desc;
  desc.matrix = NULL;
  desc.tile_size = tile_size;
  desc.matrix_nelements = matrix_nelements;
  desc.tile_nelements = tile_nelements;
  desc.matrix_size = matrix_size;
  return desc;
}

/* Matrix descriptor creation */
int tpm_matrix_desc_create(tpm_desc **desc, void *matrix, int tile_size,
                           long int matrix_nelements, long int tile_nelements,
                           int matrix_size) {
  *desc = (tpm_desc *)malloc(sizeof(tpm_desc));
  **desc = tpm_matrix_desc_init(tile_size, matrix_nelements, tile_nelements,
                                matrix_size);
  (**desc).matrix = matrix;
  return 0;
}

/* Matrix descriptor allocation */
int tpm_matrix_desc_alloc(tpm_desc *desc) {
  size_t size;
  size = desc->matrix_size * desc->matrix_size * sizeof(double);
  desc->matrix = malloc(size);
  return 0;
}

/* Matrix descriptor destruction */
int tpm_matrix_desc_destroy(tpm_desc **desc) {
  free(*desc);
  *desc = NULL;
  return 0;
}
