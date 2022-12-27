/*
 * =====================================================================================
 *
 *       Filename:  common.h
 *
 *    Description:  Common definitions used for QR
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
// clang-format on

#include "srcqr/dgeqrt.h"
#include "srcqr/dormqr.h"
#include "srcqr/dtsmqr.h"
#include "srcqr/dtsqrt.h"
