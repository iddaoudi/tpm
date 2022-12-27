/*
 * =====================================================================================
 *
 *       Filename:  qr.h
 *
 *    Description:  Control loop for QR tasks power management
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

void qr_control(int selected_case, char *task, int cpu,
                unsigned long selected_frequency,
                unsigned long original_frequency) {
  if (selected_case == 1) {
    if (!strcmp(task, "ormqr"))
      tpm_set_max_frequency(cpu, selected_frequency);
    else
      tpm_set_max_frequency(cpu, original_frequency);
  }

  if (selected_case == 2) {
    if (!strcmp(task, "tsqrt"))
      tpm_set_max_frequency(cpu, selected_frequency);
    else
      tpm_set_max_frequency(cpu, original_frequency);
  }

  if (selected_case == 3) {
    if (!strcmp(task, "tsmqr"))
      tpm_set_max_frequency(cpu, selected_frequency);
    else
      tpm_set_max_frequency(cpu, original_frequency);
  }

  if (selected_case == 4) {
    if (!strcmp(task, "geqrt"))
      tpm_set_max_frequency(cpu, selected_frequency);
    else
      tpm_set_max_frequency(cpu, original_frequency);
  }

  if (selected_case == 5) {
    if (!strcmp(task, "ormqr") || !strcmp(task, "tsqrt"))
      tpm_set_max_frequency(cpu, selected_frequency);
    else
      tpm_set_max_frequency(cpu, original_frequency);
  }

  if (selected_case == 6) {
    if (!strcmp(task, "ormqr") || !strcmp(task, "tsmqr"))
      tpm_set_max_frequency(cpu, selected_frequency);
    else
      tpm_set_max_frequency(cpu, original_frequency);
  }

  if (selected_case == 7) {
    if (!strcmp(task, "ormqr") || !strcmp(task, "geqrt"))
      tpm_set_max_frequency(cpu, selected_frequency);
    else
      tpm_set_max_frequency(cpu, original_frequency);
  }

  if (selected_case == 8) {
    if (!strcmp(task, "tsqrt") || !strcmp(task, "tsmqr"))
      tpm_set_max_frequency(cpu, selected_frequency);
    else
      tpm_set_max_frequency(cpu, original_frequency);
  }

  if (selected_case == 9) {
    if (!strcmp(task, "tsqrt") || !strcmp(task, "geqrt"))
      tpm_set_max_frequency(cpu, selected_frequency);
    else
      tpm_set_max_frequency(cpu, original_frequency);
  }

  if (selected_case == 10) {
    if (!strcmp(task, "tsmqr") || !strcmp(task, "geqrt"))
      tpm_set_max_frequency(cpu, selected_frequency);
    else
      tpm_set_max_frequency(cpu, original_frequency);
  }

  if (selected_case == 11) {
    if (!strcmp(task, "ormqr") || !strcmp(task, "tsqrt") ||
        !strcmp(task, "tsmqr"))
      tpm_set_max_frequency(cpu, selected_frequency);
    else
      tpm_set_max_frequency(cpu, original_frequency);
  }

  if (selected_case == 12) {
    if (!strcmp(task, "ormqr") || !strcmp(task, "tsqrt") ||
        !strcmp(task, "geqrt"))
      tpm_set_max_frequency(cpu, selected_frequency);
    else
      tpm_set_max_frequency(cpu, original_frequency);
  }

  if (selected_case == 13) {
    if (!strcmp(task, "ormqr") || !strcmp(task, "tsmqr") ||
        !strcmp(task, "geqrt"))
      tpm_set_max_frequency(cpu, selected_frequency);
    else
      tpm_set_max_frequency(cpu, original_frequency);
  }

  if (selected_case == 14) {
    if (!strcmp(task, "tsqrt") || !strcmp(task, "tsmqr") ||
        !strcmp(task, "geqrt"))
      tpm_set_max_frequency(cpu, selected_frequency);
    else
      tpm_set_max_frequency(cpu, original_frequency);
  }

  if (selected_case == 15) {
    if (!strcmp(task, "ormqr") || !strcmp(task, "tsqrt") ||
        !strcmp(task, "tsmqr") || !strcmp(task, "geqrt"))
      tpm_set_max_frequency(cpu, selected_frequency);
    else
      tpm_set_max_frequency(cpu, original_frequency);
  }

  if (selected_case == 16)
    tpm_set_max_frequency(cpu, original_frequency);
}
