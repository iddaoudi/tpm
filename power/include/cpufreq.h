/*
 * =====================================================================================
 *
 *       Filename:  cpufreq.h
 *
 *    Description:  CPUFreq function to get various required informations for
 * control
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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <cpufreq.h>

#define MAX_GOVERNORS 8
#define MAX_FREQUENCIES 20
#define MAX_CHARACTERS 10

struct cpufreq_available_governors *governor;
struct cpufreq_available_frequencies *frequency;
struct cpufreq_policy *policy_governor;
struct cpufreq_policy *new_policy_governor;

unsigned long *tpm_query_available_frequencies(unsigned int cpu) {
  frequency = cpufreq_get_available_frequencies(cpu);
  static unsigned long frequencies_vector[MAX_FREQUENCIES];
  int counter = 0;
  if (frequency) {
    while (frequency->next) {
      frequencies_vector[counter] = frequency->frequency;
      frequency = frequency->next;
      counter++;
    }
    frequencies_vector[counter] = frequency->frequency;
  }
  cpufreq_put_available_frequencies(frequency);
  return frequencies_vector;
}

char **tpm_query_available_governors(unsigned int cpu) {
  governor = cpufreq_get_available_governors(cpu);
  char **governors_vector = malloc(
      MAX_GOVERNORS * sizeof(char *)); // assuming a maximum of 10 governors
  for (int i = 0; i < MAX_GOVERNORS; i++) {
    governors_vector[i] = malloc(MAX_CHARACTERS * sizeof(char));
  }
  int counter = 0;
  if (governor) {
    while (governor->next) {
      strcpy(governors_vector[counter], governor->governor);
      governor = governor->next;
      counter++;
    }
    strcpy(governors_vector[counter], governor->governor);
  }
  cpufreq_put_available_governors(governor);
  return governors_vector;
}

char *tpm_query_current_governor_policy(unsigned int cpu) {
  policy_governor = cpufreq_get_policy(cpu);
  void *tmp = NULL;
  if (policy_governor) {
    tmp = malloc(strlen(policy_governor->governor) + 1);
    strcpy((char *)tmp, policy_governor->governor);
  }
  cpufreq_put_policy(policy_governor);
  return (char *)tmp;
}

double tpm_query_current_frequency_hardware(unsigned int cpu) {
  return cpufreq_get_freq_hardware(cpu) * 1e-3; // Mhz
}

double tpm_query_current_frequency_kernel(unsigned int cpu) {
  return cpufreq_get_freq_kernel(cpu) * 1e-3; // Mhz
}

void tpm_set_governor_policy(unsigned int cpu, char *new_governor) {
  int ret = cpufreq_modify_policy_governor(cpu, new_governor);
  if (ret != 0) {
    printf("No root access.\n");
    exit(EXIT_FAILURE);
  }
}

void tpm_set_max_frequency(unsigned int cpu, unsigned long max_freq) {
  int ret = cpufreq_modify_policy_max(cpu, max_freq);

  if (ret != 0) {
    printf("No root access.\n");
    exit(EXIT_FAILURE);
  }
}