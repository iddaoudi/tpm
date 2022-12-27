/*
 * =====================================================================================
 *
 *       Filename:  utils.h
 *
 *    Description:  Various useful functions
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

#include "stdio.h"
#include "string.h"

int frequencies_vector_size_counter(unsigned long *frequencies_vector) {
  int frequencies_vector_size = 0;
  while (frequencies_vector[frequencies_vector_size] != 0) {
    frequencies_vector_size++;
  }
  return frequencies_vector_size;
}

unsigned long select_frequency(char *target_frequency,
                               unsigned long *frequencies_vector,
                               int frequencies_vector_size) {
  if (strcmp(target_frequency, "mid") == 0) {
    return frequencies_vector[frequencies_vector_size / 2];
  } else if (strcmp(target_frequency, "max") == 0) {
    return frequencies_vector[0]; // first element is the last value inserted in
                                  // the array
  } else if (strcmp(target_frequency, "min") == 0) {
	 return frequencies_vector[frequencies_vector_size - 1];
  }
  return -1;
}

void logs(unsigned long *frequencies_vector, int frequencies_vector_size,
          char **governors_vector, char *original_governor,
          unsigned long selected_frequency) {
  printf("############################################\n");
  printf("Available frequencies: ");
  for (int i = 0; i < frequencies_vector_size; i++) {
    printf("%lu ", frequencies_vector[i]);
  }
  printf("\n");
  int counter = 0;
  printf("Available governors: ");
  while (strcmp(governors_vector[counter], "") != 0) {
    printf("%s ", governors_vector[counter]);
    counter++;
  }
  printf("\n");
  printf("Current governor: %s\n", original_governor);
  printf("Selected frequency: %lu\n", selected_frequency);
  printf("############################################\n");
}

void file_dump(int active_packages, uint64_t pkg_energy_start[],
               uint64_t pkg_energy_finish[], uint64_t dram_energy_start[],
               uint64_t dram_energy_finish[]) {
  int counter = 0;
  while (counter != active_packages) {
    char name[TPM_FILENAME_STRING_SIZE];
    snprintf(name, TPM_FILENAME_STRING_SIZE, "pkg_%d.dat", counter);
    FILE *file = fopen(name, "a");
    assert(file != NULL);
    fprintf(file, "%" PRIu64 " ",
            pkg_energy_finish[counter] - pkg_energy_start[counter]);
    counter++;
  }
  counter = 0;
  while (counter != active_packages) {
    char name[TPM_FILENAME_STRING_SIZE];
    snprintf(name, TPM_FILENAME_STRING_SIZE, "dram_%d.dat", counter);
    FILE *file = fopen(name, "a");
    assert(file != NULL);
    fprintf(file, "%" PRIu64 " ",
            dram_energy_finish[counter] - dram_energy_start[counter]);
    counter++;
  }
}
