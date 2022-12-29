/*
 * =====================================================================================
 *
 *       Filename:  tile_address.h
 *
 *    Description:  Tile address function
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

inline static void *tpm_tile_address(tpm_desc A, int m, int n) {
  size_t single_element_size = sizeof(double);
  size_t offset = 0;

  /* Current implementation considers that matrix size % bloc size = 0 */
  offset = A.tile_nelements * (m + A.matrix_size / A.tile_size * n);
  return (void *)((char *)A.matrix + (offset * single_element_size));
}
