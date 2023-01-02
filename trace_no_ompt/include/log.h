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

#define TPM_FILENAME_SIZE 64

int tpm_task_times_log(cvector_vector_type(tpm_task_t *) tpm_tasks) {
  char file_name[TPM_FILENAME_SIZE];
  snprintf(file_name, TPM_FILENAME_SIZE, "tasks_%s_%d_%d_%d.dat", algorithm,
           matrix_size, tile_size, n_threads);
  FILE *file = fopen(file_name, "a");
  if (file == NULL)
    return -1;

  for (int i = 0; i < cvector_size(tpm_tasks); i++) {
    double tmp = tpm_tasks[i]->end_time - tpm_tasks[i]->start_time;   // in us
	 fprintf(file, "%s, %f\n", tpm_tasks[i]->name, tmp);               // in us
  }
  return 0;
}
