/*
 * =====================================================================================
 *
 *       Filename:  tpm_trace_no_ompt.c
 *
 *    Description:  Tracing library functions without OMPT
 *
 *        Version:  1.0
 *        Created:  31/12/2022 23:37:16
 *       Revision:  none
 *       Compiler:  clang
 *
 *         Author:  Idriss Daoudi <idaoudi@anl.gov>
 *   Organization:  Argonne National Laboratory
 *
 * =====================================================================================
 */

#include <pthread.h>
#include <stddef.h>
#include <sys/time.h>

#include "tpm_trace_no_ompt.h"
#include "include/cvector.h"
#include "include/hashmap.h"

FILE *file;

bool user_iter(const void *item, void *udata) {
    tpm_task_t *user = item;
    double tmp = user->end_time - user->start_time;   // in us
	fprintf(file, "%s, %f\n", user->name, tmp*1e-3);  // in ms
    return true;
}

int user_compare(const void *a, const void *b, void *udata) {
    const tpm_task_t *ua = a;
    const tpm_task_t *ub = b;
    return strcmp(ua->name, ub->name);
}

uint64_t user_hash(const void *item, uint64_t seed0, uint64_t seed1) {
  const tpm_task_t *task = item;
  return hashmap_sip(task->name, strlen(task->name), seed0, seed1);
}

void tpm_task_create(const char *name) {
  tpm_task_t *task = (tpm_task_t *)malloc(sizeof(*task));
  task->name = malloc(TPM_STRING_SIZE * sizeof(char));
  strcpy(task->name, name);
  task->cpu = 0;
  task->node = 0;
  task->start_time = 0.0;
  task->end_time = 0.0;
  pthread_mutex_lock(&mutex);
  hashmap_set(map, task); 
  pthread_mutex_unlock(&mutex);
}

extern void tpm_trace_start(char *upstream_algorithm, int upstream_matrix_size,
                            int upstream_tile_size, int upstream_n_threads) {
  TPM_POWER = atoi(getenv("TPM_POWER"));
  if (TPM_POWER) {
    /* Intialize ZMQ context and request */
    context = zmq_ctx_new();
    request = zmq_socket(context, ZMQ_PUSH);
    /* Connect to client and send request to start energy measurements */
    tpm_zmq_connect_client(request);
    tpm_zmq_send_signal(request, "energy 0");
  }
  algorithm = upstream_algorithm;
  matrix_size = upstream_matrix_size;
  tile_size = upstream_tile_size;
  n_threads = upstream_n_threads;
  map = hashmap_new(sizeof(tpm_task_t), 0, 0, 0, user_hash, user_compare, NULL, NULL);
}

/* This upstream function is called everytime before task creation, so the
 * new_name is always ready to be inserted in the task object */
extern void tpm_trace_set_task_name(const char *name) {
  new_name = name;
  tpm_task_create(new_name);
}

/* This upstream function is called everytime inside a RUNNING task, therefore
 * the corresponding task object has already been created */
extern void tpm_trace_set_task_cpu_node(int cpu, int node, char *input_name) {
  pthread_mutex_lock(&mutex);
  char *task_and_cpu = tpm_task_and_cpu_string(input_name, cpu);
  tpm_zmq_send_signal(request, task_and_cpu);
  tpm_task_t *task = hashmap_get(map, &(tpm_task_t){ .name = input_name });
  if (task != NULL) {
    task->cpu = cpu;
    task->node = node;
  }
  pthread_mutex_unlock(&mutex);
}

extern void tpm_trace_get_task_time(struct timeval start, struct timeval end,
                                    char *name) {
  pthread_mutex_lock(&mutex);
  tpm_task_t *task = hashmap_get(map, &(tpm_task_t){ .name = name });
  if (task != NULL) {
    task->start_time = (start.tv_sec) * 1000000 + start.tv_usec;
    task->end_time = (end.tv_sec) * 1000000 + end.tv_usec;
  }
  pthread_mutex_unlock(&mutex);
}

extern void tpm_trace_finalize() {
  char file_name[TPM_FILENAME_SIZE];
  snprintf(file_name, TPM_FILENAME_SIZE, "tasks_%s_%d_%d_%d.dat", algorithm,
           matrix_size, tile_size, n_threads);
  file = fopen(file_name, "a");
  if (file == NULL) {
      printf("file problem\n");
  }
  hashmap_scan(map, user_iter, file);
  pthread_mutex_destroy(&mutex);
  hashmap_free(map);

  /* Send request to end energy measurements and close the socket connexion */
  tpm_zmq_send_signal(request, "energy 1");
  tpm_zmq_send_signal(request, "end");
  tpm_zmq_close(request, context);
}
