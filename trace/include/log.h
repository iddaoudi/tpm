/*
 * =====================================================================================
 *
 *       Filename:  log.h
 *
 *    Description:  File dump functions
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

void tpm_log_trace(cvector_vector_type(tpm_task_t *) ompt_tasks) {
  for (int i = 0; i < cvector_size(ompt_tasks); i++) {
    printf("Task ID          : %ld\n", ompt_tasks[i]->id);
    printf("Task name        : %s\n", ompt_tasks[i]->name);
    printf("Task ndeps       : %d\n", ompt_tasks[i]->n_dependences);
    printf("Task access mode : ");
    for (int j = 0; j < ompt_tasks[i]->n_dependences; j++) {
      printf("%s ", ompt_tasks[i]->access_mode[j]);
    }
    printf("\n");
    printf("Task scheduled   : %d\n", ompt_tasks[i]->scheduled);
    printf("Task start time  : %f\n", ompt_tasks[i]->start_time);
    printf("Task end time    : %f\n", ompt_tasks[i]->end_time);
    printf("Task cpu         : %d\n", ompt_tasks[i]->cpu);
    printf("Task node        : %d\n", ompt_tasks[i]->node);
    printf("Task dependences : ");
    for (int j = 0; j < ompt_tasks[i]->n_task_dependences; j++) {
      for (int k = 0; k < cvector_size(ompt_tasks); k++) {
        if (ompt_tasks[i]->task_dependences[j] == ompt_tasks[k]->id) {
          printf("%s ", ompt_tasks[k]->name);
        }
      }
    }
    printf("\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  }
}

int tpm_task_times_trace(cvector_vector_type(tpm_task_t *) ompt_tasks) {
  char file_name[TPM_FILENAME_SIZE];
  snprintf(file_name, TPM_FILENAME_SIZE, "tasks_%s_%d_%d_%d.dat", ALG,
           MSIZE, BSIZE, NTH);
  FILE *file = fopen(file_name, "a");
  if (file == NULL)
    return -1;

  for (int i = 0; i < cvector_size(ompt_tasks); i++) {
    double tmp = ompt_tasks[i]->end_time - ompt_tasks[i]->start_time; // in us
	 fprintf(file, "%s, %f\n", ompt_tasks[i]->name, tmp / 1000);       // in ms
  }
  return 0;
}
