/*
 * =====================================================================================
 *
 *       Filename:  trace.h
 *
 *    Description:  OMPT tracing functions
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

/* ompt_callback_task_create is used for callbacks that are dispatched when task
 * regions or initial tasks are generated */
static void
trace_ompt_callback_task_create(ompt_data_t *encountering_task_data,
                                const ompt_frame_t *encountering_task_frame,
                                ompt_data_t *new_task_data, int flags,
                                int has_dependences, const void *codeptr_ra) {
  unsigned int thread_id = ompt_get_thread_data()->value;
  /* All tasks except first and last ones */
  if (flags & ompt_task_explicit)
    c_counter[thread_id]._cc.task_create_explicit += 1;
  /* Final task */
  if (flags & ompt_task_final)
    c_counter[thread_id]._cc.task_create_finalize += 1;
  new_task_data->value = ompt_get_unique_id();

  tpm_task_t *task = (tpm_task_t *)malloc(sizeof(*task));
  new_task_data->ptr = task;
  task->id = new_task_data->value;
  task->name = malloc(TPM_STRING_SIZE * sizeof(char));
  strcpy(task->name, new_name);
  task->task_ptr = codeptr_ra;
  task->n_dependences = 0;
  task->dependences = NULL;
  task->start_time = 0;
  task->end_time = 0;
  task->scheduled = false;
  task->n_task_dependences = 0;
  task->cpu = 0;
  task->node = 0;
  pthread_mutex_lock(&mutex);
  cvector_push_back(ompt_tasks, task);
  pthread_mutex_unlock(&mutex);
}

#ifdef TRACE_TASK_SCHEDULE
// ompt_callback_task_schedule is used for callbacks that are dispatched when
// task scheduling decisions are made
static void
trace_ompt_callback_task_schedule(ompt_data_t *prior_task_data,
                                  ompt_task_status_t prior_task_status,
                                  ompt_data_t *next_task_data) {
  unsigned int thread_id = ompt_get_thread_data()->value;
  c_counter[thread_id]._cc.task_schedule += 1;
  tpm_task_t *task;
  for (int i = 0; i < cvector_size(ompt_tasks); i++) {
    if (ompt_tasks[i]->id == prior_task_data->value) {
      task = ompt_tasks[i];
      pthread_mutex_lock(&mutex);
      task->scheduled = true;
      pthread_mutex_unlock(&mutex);
      break;
    }
  }
}
#endif

// ompt_callback_dependences is used for callbacks that are related to
// dependences and that are dispatched when new tasks are generated
static void trace_ompt_callback_dependences(ompt_data_t *task_data,
                                            const ompt_dependence_t *deps,
                                            int ndeps) {
  unsigned int thread_id = ompt_get_thread_data()->value;
  c_counter[thread_id]._cc.dependences += 1;
  tpm_task_t *task;
  // Find the task
  for (int i = 0; i < cvector_size(ompt_tasks); i++) {
    if (ompt_tasks[i]->id == task_data->value) {
      task = ompt_tasks[i];
      break;
    }
  }
  pthread_mutex_lock(&mutex);
  task->dependences =
      (ompt_dependence_t *)malloc(ndeps * sizeof(ompt_dependence_t));
  task->n_dependences = ndeps;
  memcpy(task->dependences, deps, ndeps * sizeof(ompt_dependence_t));
  assert(ndeps < MAX_MODES);
  for (int i = 0; i < ndeps; i++) {
    if (task->dependences[i].dependence_type == 1)
      strcpy(task->access_mode[i], "R");
    else if (task->dependences[i].dependence_type == 2)
      strcpy(task->access_mode[i], "W");
    else if (task->dependences[i].dependence_type == 3)
      strcpy(task->access_mode[i], "RW");
    else {
      printf("Unsupported access mode. Aborting...\n");
      exit(EXIT_FAILURE);
    }
  }
  pthread_mutex_unlock(&mutex);
}

static void trace_ompt_callback_task_dependence(ompt_data_t *src_task_data,
                                                ompt_data_t *sink_task_data) {
  unsigned int thread_id = ompt_get_thread_data()->value;
  c_counter[thread_id]._cc.task_dependence += 1;

  pthread_mutex_lock(&mutex);
  tpm_task_t *src_task = NULL;
  tpm_task_t *sink_task = NULL;
  /* Find the task */
  for (int i = 0; i < cvector_size(ompt_tasks); i++) {
    if (ompt_tasks[i]->id == src_task_data->value) {
      src_task = ompt_tasks[i];
      break;
    }
  }
  for (int i = 0; i < cvector_size(ompt_tasks); i++) {
    if (ompt_tasks[i]->id == sink_task_data->value) {
      sink_task = ompt_tasks[i];
      break;
    }
  }
  if (sink_task != NULL && src_task != NULL) {
    int count = sink_task->n_task_dependences;
    sink_task->task_dependences[count] = src_task->id;
    sink_task->n_task_dependences += 1;
  }
  pthread_mutex_unlock(&mutex);
}
