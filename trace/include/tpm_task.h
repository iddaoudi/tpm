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
  const void *task_ptr;
  char *name;
  uint64_t id;
  int n_dependences;
  ompt_dependence_t *dependences;
  char access_mode[MAX_MODES][TPM_STRING_SIZE];
  double start_time; // in us
  double end_time;
  bool scheduled;
  int n_task_dependences;
  int task_dependences[MAX_MODES];
  int cpu;
  int node;
} tpm_task_t;

cvector_vector_type(tpm_task_t *) ompt_tasks = NULL;
