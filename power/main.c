/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  Power management for task-based algorithms
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

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <zmq.h>

#define TPM_STRING_SIZE 16
#define TPM_TASK_STRING_SIZE 8
#define TPM_FILENAME_STRING_SIZE 16

// clang-format off
#include "include/utils.h"
#include "include/cpufreq.h"
#include "include/rapl.h"
#include "include/server.h"

#include "control/cholesky.h"
#include "control/qr.h"
#include "control/lu.h"
#include "control/sparselu.h"
// clang-format on

int MAX_CPUS, selected_case;
char *input_governor, *target_frequency, *algorithm;

int start_exp() {
  void *context = zmq_ctx_new();
  void *server = zmq_socket(context, ZMQ_PULL);

  int ret = tpm_zmq_connect_server(server);
  assert(ret == 0);

  /* Initialization */
  int active_packages = rapl_init();

  /* Get current governor policy */
  char *original_governor = tpm_query_current_governor_policy(0);

  /* Get all possible frequencies (in KHz) and all available governors */
  unsigned long *frequencies_vector = tpm_query_available_frequencies(0);
  char **governors_vector = tpm_query_available_governors(0);

  int frequencies_vector_size =
      frequencies_vector_size_counter(frequencies_vector);
  /* By default, the original frequency of every CPU is set to its maximum */
  unsigned long original_frequency = frequencies_vector[0];
  unsigned long selected_frequency = select_frequency(
      target_frequency, frequencies_vector, frequencies_vector_size);
  assert(frequencies_vector_size != 0);
  assert(original_frequency != 0);
  assert(selected_frequency != -1);

#ifdef LOG
  logs(frequencies_vector, frequencies_vector_size, governors_vector,
       original_governor, selected_frequency);
#endif

  uint64_t pkg_energy_start[active_packages];
  uint64_t pkg_energy_finish[active_packages];
  uint64_t dram_energy_start[active_packages];
  uint64_t dram_energy_finish[active_packages];

  /* Change the governor */
  for (int i = 0; i < MAX_CPUS; i++)
    tpm_set_governor_policy(i, input_governor);

  while (1) {
    char task_and_cpu[TPM_STRING_SIZE];
    char task[TPM_TASK_STRING_SIZE];
    int cpu = 0;

    /* Receive current task name and its CPU */
    zmq_recv(server, task_and_cpu, TPM_STRING_SIZE, 0);
    sscanf(task_and_cpu, "%s %d", task, &cpu);

    /* Frequency control */
    if (!strcmp(algorithm, "cholesky"))
      cholesky_control(selected_case, task, cpu, selected_frequency,
                       original_frequency);
    else if (!strcmp(algorithm, "qr"))
      qr_control(selected_case, task, cpu, selected_frequency,
                 original_frequency);
    else if (!strcmp(algorithm, "lu"))
      lu_control(selected_case, task, cpu, selected_frequency,
                 original_frequency);
    else if (!strcmp(algorithm, "sparselu"))
      sparselu_control(selected_case, task, cpu, selected_frequency,
                 original_frequency);
    
    if (strcmp(task, "energy") == 0) {
      /* Start measuring energy */
      if (cpu == 0) {
        for (int i = 0; i < active_packages; i++) {
          pkg_energy_start[i] = rapl_readenergy_uj(i, "pkg");
          dram_energy_start[i] = rapl_readenergy_uj(i, "dram");
          assert(pkg_energy_start[i] < check_max(i, "pkg"));
          assert(dram_energy_start[i] < check_max(i, "dram"));
        }
      }
      /* End measuring energy */
      else if (cpu == 1) {
        for (int i = 0; i < active_packages; i++) {
          pkg_energy_finish[i] = rapl_readenergy_uj(i, "pkg");
          dram_energy_finish[i] = rapl_readenergy_uj(i, "dram");
          /* Make sure measurements take into account the maximum energy value
           */
          if (pkg_energy_finish[i] < pkg_energy_start[i])
            pkg_energy_finish[i] += check_max(i, "pkg");
          if (dram_energy_finish[i] < dram_energy_start[i])
            dram_energy_finish[i] += check_max(i, "dram");
        }
      }
    }
    /* End measurements */
    if (strcmp(task_and_cpu, "end") == 0)
      break;
  }

  /* Set back the original governor policy and frequency (max by default) */
  for (int i = 0; i < MAX_CPUS; i++) {
    tpm_set_governor_policy(i, original_governor);
    tpm_set_max_frequency(i, original_frequency); // KHz
  }

  file_dump(active_packages, pkg_energy_start, pkg_energy_finish,
            dram_energy_start, dram_energy_finish);
  printf("%lu\n", selected_frequency);

  return 0;
}

int main(int argc, char *argv[]) {
  if (argc < 6 || argc > 6) {
    printf("Server side arguments problem.\n");
    exit(EXIT_FAILURE);
  } else {
    MAX_CPUS = atoi(argv[1]);      // number of cpus to use
    input_governor = argv[2];      // wanted governor
    target_frequency = argv[3];    // mid or max for now FIXME
    selected_case = atoi(argv[4]); // combination to select
    algorithm = argv[5];
  }
  int ret = start_exp();
  assert(ret == 0);
}
