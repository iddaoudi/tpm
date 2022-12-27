/*
 * =====================================================================================
 *
 *       Filename:  upstream.h
 *
 *    Description:  Upstream trace functions
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

/* This upstream function is called everytime before task creation, so the
 * new_name is always ready to be inserted in the task object */
extern void tpm_trace_set_task_name(const char *name) { new_name = name; }

/* This upstream function is called everytime inside a RUNNING task, therefore
 * the corresponding task object has already been created */
extern void tpm_trace_set_task_cpu_node(int cpu, int node, char *name) {
  pthread_mutex_lock(&mutex);

  char *task_and_cpu = tpm_task_and_cpu_string(name, cpu);
  tpm_zmq_send_signal(request, task_and_cpu);

  for (int i = 0; i < cvector_size(ompt_tasks); i++) {
    if (strcmp(ompt_tasks[i]->name, name) == 0) {
      ompt_tasks[i]->cpu = cpu;
      ompt_tasks[i]->node = node;
    }
  }
  free(task_and_cpu);
  pthread_mutex_unlock(&mutex);
}

extern void tpm_trace_get_task_time(struct timeval start, struct timeval end,
                                    char *name) {
  pthread_mutex_lock(&mutex);
  for (int i = 0; i < cvector_size(ompt_tasks); i++) {
	  if (strcmp(ompt_tasks[i]->name, name) == 0) {
      /* Convert to us */
      ompt_tasks[i]->start_time = (start.tv_sec) * 1000000 + start.tv_usec;
      ompt_tasks[i]->end_time = (end.tv_sec) * 1000000 + end.tv_usec;
	 }
  }
  pthread_mutex_unlock(&mutex);
}
