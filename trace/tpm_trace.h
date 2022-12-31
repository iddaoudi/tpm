/*
 * =====================================================================================
 *
 *       Filename:  tpm_trace.h
 *
 *    Description:  Tracing library definitions
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
#include <inttypes.h>
#include <omp-tools.h>
#include <omp.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "include/cvector.h"

/* Create context and request objects needed by ZMQ */
void *context;
void *request;

/*  Parameters for file dump */
char *ALG = NULL;
int MSIZE = 0;
int BSIZE = 0;
int NTH = 0;
int TPM_POWER = 0;

// clang-format off
#include "include/common.h"
#include "include/callbacks.h"
#include "include/client.h"
// clang-format on

extern void tpm_trace_start(char *algorithm, int matrix_size, int tile_size,
                            int nthreads);
extern void tpm_trace_set_task_name(const char *name);
extern void tpm_trace_set_task_cpu_node(int cpu, int node, char *name);
extern void tpm_trace_get_task_time(struct timeval start, struct timeval end,
                                    char *name);

pthread_mutex_t mutex;

static ompt_get_thread_data_t ompt_get_thread_data;
static ompt_get_unique_id_t ompt_get_unique_id;
static ompt_get_proc_id_t ompt_get_proc_id;
static ompt_get_place_num_t ompt_get_place_num;

callback_counter_type *c_counter;

extern void tpm_start(char *algorithm, int matrix_size, int tile_size,
                      int nthreads) {
  tpm_trace_start(algorithm, matrix_size, tile_size, nthreads);
}

extern void tpm_set_task_name(const char *name) {
  tpm_trace_set_task_name(name);
}

extern void tpm_set_task_cpu_node(int cpu, int node, char *name) {
  tpm_trace_set_task_cpu_node(cpu, node, name);
}

extern void tpm_get_task_time(struct timeval start, struct timeval end,
                              char *name) {
  tpm_trace_get_task_time(start, end, name);
}

// clang-format off
#include "include/tpm_task.h"
#include "include/trace.h"
#include "include/upstream.h"
// clang-format on

int ompt_initialize(ompt_function_lookup_t lookup, int initial_device_num,
                    ompt_data_t *data_from_tool);

void ompt_finalize(ompt_data_t *data_from_tool);

ompt_start_tool_result_t *ompt_start_tool(unsigned int omp_version,
                                          const char *runtime_version);
