/*
 * =====================================================================================
 *
 *       Filename:  utils.h
 *
 *    Description:  Headers imports and useful definitions used across dense algorithms
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

#include "assert.h"
#include "cblas.h"
#include "lapacke.h"
#include <omp.h>

int MSIZE, BSIZE, NTH, TPM_TRACE, TPM_TRACE_NO_OMPT;

#define A(m, n) tpm_tile_address(A, m, n)
#define B(m, n) tpm_tile_address(B, m, n)
#define S(m, n) tpm_tile_address(S, m, n)

#define tpm_upper 121
#define tpm_lower 122
#define tpm_left 141
#define tpm_right 142
#define tpm_notranspose 111
#define tpm_transpose 112
#define tpm_nonunit 131
#define tpm_unit 132
#define tpm_W 501
#define tpm_A2 502
#define tpm_row 101
#define tpm_column 102
#define tpm_forward 391
#define tpm_backward 392

// clang-format off
#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))

#include "cvector.h"
#include "descriptor.h"
#include "tile_address.h"
#include "populate.h"
#include "print.h"

#include "../dense/cholesky.h"

#include "../dense/srcqr/dgeqrt.h"
#include "../dense/srcqr/dormqr.h"
#include "../dense/srcqr/dtsmqr.h"
#include "../dense/srcqr/dtsqrt.h"
#include "../dense/qr.h"

#include "../dense/srclu/dgetrf.h"
#include "../dense/lu.h"

#include "../sparse/srcslu/empty_block.h"
#include "../sparse/srcslu/bdiv.h"
#include "../sparse/srcslu/bmod.h"
#include "../sparse/srcslu/lu0.h"
#include "../sparse/srcslu/fwd.h"
#include "../sparse/sparselu.h"
// clang-format on
