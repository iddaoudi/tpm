/*
 * =====================================================================================
 *
 *       Filename:  utils.h
 *
 *    Description:  Useful definitions used across dense algorithms
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

#define _GNU_SOURCE

#include <getopt.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "cblas.h"
#include "lapacke.h"
#include <omp.h>

int MSIZE, BSIZE, NTH, TPM_TRACE;

#define A(m, n) MATRIX_tile_address(A, m, n)
#define B(m, n) MATRIX_tile_address(B, m, n)
#define S(m, n) MATRIX_tile_address(S, m, n)

// clang-format off
#include "cvector.h"
#include "descriptor.h"
#include "tile_address.h"
#include "populate.h"
#include "print.h"

#include "../dense/cholesky.h"
#include "../dense/qr.h"
#include "../dense/lu.h"
// clang-format on

#include "../../power/include/rapl.h"
