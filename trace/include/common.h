/*
 * =====================================================================================
 *
 *       Filename:  common.h
 *
 *    Description:  Common functions and definitions
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

#define TPM_STRING_SIZE 16
#define TPM_FILENAME_SIZE 32

/* A million tasks is enough for now, FIXME */
#define MAX_TASKS 1000000
#define MAX_MODES 128
#define MAX_THREADS 128
#define CACHE_LINE 128

static ompt_get_thread_data_t ompt_get_thread_data;
static ompt_get_unique_id_t ompt_get_unique_id;
static ompt_get_proc_id_t ompt_get_proc_id;
static ompt_get_place_num_t ompt_get_place_num;

pthread_mutex_t mutex;
const char *new_name = NULL;

void *context = NULL;
void *request = NULL;

char *tpm_task_and_cpu_string(char *task, int cpu) {
  int char_counter = 0;
  char *task_name = (char *)malloc(TPM_STRING_SIZE * sizeof(char));
  char *tmp_name = (char *)malloc(TPM_STRING_SIZE * sizeof(char));
  memset(task_name, '\0', TPM_STRING_SIZE);
  memset(tmp_name, '\0', TPM_STRING_SIZE);
  size_t task_size = strlen(task);
  for (int i = 0; i < task_size; i++) {
    if (isalpha(task[i]))
      char_counter++;
    else
      break;
  }
  strncpy(tmp_name, task, char_counter);
  tmp_name[char_counter + 1] = '\0';
  snprintf(task_name, TPM_STRING_SIZE, "%s %d", tmp_name, cpu);
  free(tmp_name);

  return task_name;
}
