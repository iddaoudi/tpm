/*
 * =====================================================================================
 *
 *       Filename:  tpm_trace.c
 *
 *    Description:  Tracing library functions
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

#include <ctype.h>
#include <stddef.h>
#include <sys/time.h>

// clang-format off
#include "tpm_trace.h"
#include "include/log.h"
// clang-format on

#define LOG 1

/* Parameters for file dump */
extern void tpm_trace_start(char *algorithm, int matrix_size, int tile_size,
                            int nthreads) {
  ALG = algorithm;
  MSIZE = matrix_size;
  BSIZE = tile_size;
  NTH = nthreads;
}

int ompt_initialize(ompt_function_lookup_t lookup, int initial_device_num,
                    ompt_data_t *data_from_tool) {
  /* Intialize ZMQ context and request */
  context = zmq_ctx_new();
  request = zmq_socket(context, ZMQ_PUSH);
  /* Connect to client and send request to start energy measurements */
  tpm_zmq_connect_client(request);
  tpm_zmq_send_signal(request, "energy 0");

  /* Runtime entrypoint */
  ompt_set_callback_t ompt_set_callback =
      (ompt_set_callback_t)lookup("ompt_set_callback");
  ompt_get_thread_data = (ompt_get_thread_data_t)lookup("ompt_get_thread_data");
  ompt_get_unique_id = (ompt_get_unique_id_t)lookup("ompt_get_unique_id");
  ompt_get_proc_id = (ompt_get_proc_id_t)lookup("ompt_get_proc_id");

  /* Callbacks registering */
  register_callback(ompt_callback_task_create);
  register_callback(ompt_callback_dependences);
  register_callback(ompt_callback_task_dependence);
#ifdef TRACE_TASK_SCHEDULE
  register_callback(ompt_callback_task_schedule);
#endif

  c_counter = calloc(MAX_THREADS, sizeof(callback_counter_type));
  data_from_tool->ptr = c_counter;

  return 1;
}

void ompt_finalize(ompt_data_t *data_from_tool) {
  sum_of_callbacks(data_from_tool->ptr);
#ifdef LOG
  // tpm_log_trace(ompt_tasks);
  int ret = tpm_task_times_trace(ompt_tasks);
  assert(ret == 0);
#endif
  for (int i = 0; i < cvector_size(ompt_tasks); i++) {
    free(ompt_tasks[i]->dependences);
    free(ompt_tasks[i]->name);
    free(ompt_tasks[i]);
  }
  free(data_from_tool->ptr);
  pthread_mutex_destroy(&mutex);

  /* Send request to end energy measurements and close the socket connexion */
  tpm_zmq_send_signal(request, "energy 1");
  tpm_zmq_send_signal(request, "end");
  tpm_zmq_close(request, context);
}

ompt_start_tool_result_t *ompt_start_tool(unsigned int omp_version,
                                          const char *runtime_version) {
  static ompt_start_tool_result_t ompt_start_tool_result = {
      &ompt_initialize, &ompt_finalize, {.value = 0}};
  return &ompt_start_tool_result;
}
