/*
 * =====================================================================================
 *
 *       Filename:  callbacks.h
 *
 *    Description:  OMPT callback functions
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
#include <sys/resource.h>

/* Callback registration macro using concatenation to convert callback functions
 * names */
#define register_callback_t(name, type)                                        \
  do {                                                                         \
    type f_##name = &trace_##name;                                             \
    if (ompt_set_callback(name, (ompt_callback_t)f_##name) == ompt_set_never)  \
      printf("Callback '" #name "' could not be registered.\n");               \
  } while (0)

#define register_callback(name) register_callback_t(name, name##_t)

/* Callback counter structure definition */
typedef struct {
  int parallel_begin;
  int parallel_end;
  int thread_begin;
  int thread_end;
  int task_create_initialize;
  int task_create_implicit;
  int task_create_explicit;
  int task_create_finalize;
  int task_schedule;
  int task_dependence;
  int dependences;
  int implicit_task_scope_begin;
  int implicit_task_scope_end;
  int initial_task_scope_begin;
  int initial_task_scope_end;
} callback_counter;

typedef union __attribute__((aligned(CACHE_LINE))) callback_counter_union {
  callback_counter _cc;
} callback_counter_type;

/* Sum of callbacks counters */
void sum_of_callbacks(callback_counter_type *c_counter) {
  for (int i = 0; i < MAX_THREADS; i++) {
    c_counter[0]._cc.parallel_begin += c_counter[i]._cc.parallel_begin;
    c_counter[0]._cc.parallel_end += c_counter[i]._cc.parallel_end;
    c_counter[0]._cc.thread_begin += c_counter[i]._cc.thread_begin;
    c_counter[0]._cc.thread_end += c_counter[i]._cc.thread_end;

    c_counter[0]._cc.initial_task_scope_begin +=
        c_counter[i]._cc.initial_task_scope_begin;
    c_counter[0]._cc.initial_task_scope_end +=
        c_counter[i]._cc.initial_task_scope_end;
    c_counter[0]._cc.implicit_task_scope_begin +=
        c_counter[i]._cc.implicit_task_scope_begin;
    c_counter[0]._cc.implicit_task_scope_end +=
        c_counter[i]._cc.implicit_task_scope_end;
    c_counter[0]._cc.task_create_explicit +=
        c_counter[i]._cc.task_create_explicit;
    c_counter[0]._cc.task_create_finalize +=
        c_counter[i]._cc.task_create_finalize;
    c_counter[0]._cc.task_schedule += c_counter[i]._cc.task_schedule;
    c_counter[0]._cc.task_dependence += c_counter[i]._cc.task_dependence;
    c_counter[0]._cc.dependences += c_counter[i]._cc.dependences;
  }
}