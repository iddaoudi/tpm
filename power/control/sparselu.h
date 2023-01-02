/*
 * =====================================================================================
 *
 *       Filename:  sparselu.h
 *
 *    Description:  Control loop for SparseLU tasks power management
 *
 *        Version:  1.0
 *        Created:  03/01/2023 00:09:22
 *       Revision:  none
 *       Compiler:  clang
 *
 *         Author:  Idriss Daoudi <idaoudi@anl.gov>
 *   Organization:  Argonne National Laboratory
 *
 * =====================================================================================
 */

void sparselu_control(int selected_case, char *task, int cpu,
                      unsigned long selected_frequency,
                      unsigned long original_frequency) {
  if (selected_case == 1) {
    if (!strcmp(task, "lu0"))
      tpm_set_max_frequency(cpu, selected_frequency);
    else
      tpm_set_max_frequency(cpu, original_frequency);
  }

  if (selected_case == 2) {
    if (!strcmp(task, "fwd"))
      tpm_set_max_frequency(cpu, selected_frequency);
    else
      tpm_set_max_frequency(cpu, original_frequency);
  }

  if (selected_case == 3) {
    if (!strcmp(task, "bdiv"))
      tpm_set_max_frequency(cpu, selected_frequency);
    else
      tpm_set_max_frequency(cpu, original_frequency);
  }

  if (selected_case == 4) {
    if (!strcmp(task, "bmod"))
      tpm_set_max_frequency(cpu, selected_frequency);
    else
      tpm_set_max_frequency(cpu, original_frequency);
  }

  if (selected_case == 5) {
    if (!strcmp(task, "lu0") || !strcmp(task, "fwd"))
      tpm_set_max_frequency(cpu, selected_frequency);
    else
      tpm_set_max_frequency(cpu, original_frequency);
  }

  if (selected_case == 6) {
    if (!strcmp(task, "lu0") || !strcmp(task, "bdiv"))
      tpm_set_max_frequency(cpu, selected_frequency);
    else
      tpm_set_max_frequency(cpu, original_frequency);
  }

  if (selected_case == 7) {
    if (!strcmp(task, "lu0") || !strcmp(task, "bmod"))
      tpm_set_max_frequency(cpu, selected_frequency);
    else
      tpm_set_max_frequency(cpu, original_frequency);
  }

  if (selected_case == 8) {
    if (!strcmp(task, "fwd") || !strcmp(task, "bdiv"))
      tpm_set_max_frequency(cpu, selected_frequency);
    else
      tpm_set_max_frequency(cpu, original_frequency);
  }

  if (selected_case == 9) {
    if (!strcmp(task, "fwd") || !strcmp(task, "bmod"))
      tpm_set_max_frequency(cpu, selected_frequency);
    else
      tpm_set_max_frequency(cpu, original_frequency);
  }

  if (selected_case == 10) {
    if (!strcmp(task, "bdiv") || !strcmp(task, "bmod"))
      tpm_set_max_frequency(cpu, selected_frequency);
    else
      tpm_set_max_frequency(cpu, original_frequency);
  }

  if (selected_case == 11) {
    if (!strcmp(task, "lu0") || !strcmp(task, "fwd") ||
        !strcmp(task, "bdiv"))
      tpm_set_max_frequency(cpu, selected_frequency);
    else
      tpm_set_max_frequency(cpu, original_frequency);
  }

  if (selected_case == 12) {
    if (!strcmp(task, "lu0") || !strcmp(task, "fwd") ||
        !strcmp(task, "bmod"))
      tpm_set_max_frequency(cpu, selected_frequency);
    else
      tpm_set_max_frequency(cpu, original_frequency);
  }

  if (selected_case == 13) {
    if (!strcmp(task, "lu0") || !strcmp(task, "bdiv") ||
        !strcmp(task, "bmod"))
      tpm_set_max_frequency(cpu, selected_frequency);
    else
      tpm_set_max_frequency(cpu, original_frequency);
  }

  if (selected_case == 14) {
    if (!strcmp(task, "fwd") || !strcmp(task, "bdiv") || !strcmp(task, "bmod"))
      tpm_set_max_frequency(cpu, selected_frequency);
    else
      tpm_set_max_frequency(cpu, original_frequency);
  }

  if (selected_case == 15) {
    if (!strcmp(task, "lu0") || !strcmp(task, "fwd") ||
        !strcmp(task, "bdiv") || !strcmp(task, "bmod"))
      tpm_set_max_frequency(cpu, selected_frequency);
    else
      tpm_set_max_frequency(cpu, original_frequency);
  }

  if (selected_case == 16)
    tpm_set_max_frequency(cpu, original_frequency);
 
  /* Set back the original frequency on the CPU used by a task */
  tpm_set_max_frequency(cpu, original_frequency);
}
