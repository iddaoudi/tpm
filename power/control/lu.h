/*
 * =====================================================================================
 *
 *       Filename:  lu.h
 *
 *    Description:  Control loop for LU tasks power management
 *
 *        Version:  1.0
 *        Created:  25/12/2022 22:07:00
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Idriss Daoudi <idaoudi@anl.gov>
 *   Organization:  idaoudi@anl.gov
 *
 * =====================================================================================
 */

void lu_control(int selected_case, char *task, int cpu,
                unsigned long selected_frequency,
                unsigned long original_frequency) {
  if (selected_case == 1) {
    if (!strcmp(task, "getrf"))
      tpm_set_max_frequency(cpu, selected_frequency);
    else
      tpm_set_max_frequency(cpu, original_frequency);
  }

  if (selected_case == 2) {
    if (!strcmp(task, "gemm"))
      tpm_set_max_frequency(cpu, selected_frequency);
    else
      tpm_set_max_frequency(cpu, original_frequency);
  }

  if (selected_case == 3) {
    if (!strcmp(task, "trsm"))
      tpm_set_max_frequency(cpu, selected_frequency);
    else
      tpm_set_max_frequency(cpu, original_frequency);
  }

  if (selected_case == 4) {
    if (!strcmp(task, "getrf") || !strcmp(task, "trsm"))
      tpm_set_max_frequency(cpu, selected_frequency);
    else
      tpm_set_max_frequency(cpu, original_frequency);
  }

  if (selected_case == 5) {
    if (!strcmp(task, "getrf") || !strcmp(task, "gemm"))
      tpm_set_max_frequency(cpu, selected_frequency);
    else
      tpm_set_max_frequency(cpu, original_frequency);
  }

  if (selected_case == 6) {
    if (!strcmp(task, "trsm") || !strcmp(task, "gemm"))
      tpm_set_max_frequency(cpu, selected_frequency);
    else
      tpm_set_max_frequency(cpu, original_frequency);
  }

  if (selected_case == 7) {
    if (!strcmp(task, "gemm") || !strcmp(task, "trsm") ||
        !strcmp(task, "getrf"))
      tpm_set_max_frequency(cpu, selected_frequency);
    else
      tpm_set_max_frequency(cpu, original_frequency);
  }

  if (selected_case == 8)
    tpm_set_max_frequency(cpu, original_frequency);
}
