/*
 * =====================================================================================
 *
 *       Filename:  tpm_task.h
 *
 *    Description:  Task structure definition
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

typedef struct {
  char *name;
  double start_time; // in us
  double end_time;
  int cpu;
  int node;
  int order;
} tpm_task_t;

