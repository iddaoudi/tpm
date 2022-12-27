/*
 * =====================================================================================
 *
 *       Filename:  idle.c
 *
 *    Description:  Measuring IDLE power
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
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "include/rapl.h"

uint64_t pkg_energy_start_0,
    pkg_energy_finish_0; // FIXME dynamic package number
uint64_t pkg_energy_start_1, pkg_energy_finish_1;
uint64_t dram_energy_start_0, dram_energy_start_1;
uint64_t dram_energy_finish_0, dram_energy_finish_1;

int main(int argc, char *argv[]) {
  char *duration_string = argv[1];
  int duration = atoi(duration_string);

  rapl_init();

  pkg_energy_start_0 = rapl_readenergy_uj(0, "pkg");
  pkg_energy_start_1 = rapl_readenergy_uj(1, "pkg");
  dram_energy_start_0 = rapl_readenergy_uj(0, "dram");
  dram_energy_start_1 = rapl_readenergy_uj(1, "dram");

  assert(pkg_energy_start_0 < check_max(0, "pkg"));
  assert(pkg_energy_start_1 < check_max(1, "pkg"));
  assert(dram_energy_start_0 < check_max(0, "dram"));
  assert(dram_energy_start_1 < check_max(1, "dram"));

  sleep(duration);

  pkg_energy_finish_0 = rapl_readenergy_uj(0, "pkg");
  pkg_energy_finish_1 = rapl_readenergy_uj(1, "pkg");
  dram_energy_finish_0 = rapl_readenergy_uj(0, "dram");
  dram_energy_finish_1 = rapl_readenergy_uj(1, "dram");

  if (pkg_energy_finish_0 < pkg_energy_start_0)
    pkg_energy_finish_0 += check_max(0, "pkg") - pkg_energy_start_0;
  if (pkg_energy_finish_1 < pkg_energy_start_1)
    pkg_energy_finish_1 += check_max(1, "pkg") - pkg_energy_start_1;
  if (dram_energy_finish_0 < dram_energy_start_0)
    dram_energy_finish_0 += check_max(0, "dram") - dram_energy_start_0;
  if (dram_energy_finish_1 < dram_energy_start_1)
    dram_energy_finish_1 += check_max(1, "dram") - dram_energy_start_1;

  printf("pkg0, %" PRIu64 "\n", pkg_energy_finish_0 - pkg_energy_start_0);
  printf("pkg1, %" PRIu64 "\n", pkg_energy_finish_1 - pkg_energy_start_1);
  printf("dram0, %" PRIu64 "\n", dram_energy_finish_0 - dram_energy_start_0);
  printf("dram1, %" PRIu64 "\n", dram_energy_finish_1 - dram_energy_start_1);

  return 0;
}