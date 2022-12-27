/*
 * =====================================================================================
 *
 *       Filename:  rapl.h
 *
 *    Description:  RAPL functions to get various informations required for
 * control
 *
 *        Version:  1.0
 *        Created:  25/12/2022 19:30:35
 *       Revision:  none
 *       Compiler:  clang
 *
 *         Author:  Kazutomo Yoshii <kazutomo@mcs.anl.gov>, Idriss Daoudi
 * <idaoudi@anl.gov> Organization:  Argonne National Laboratory
 *
 * =====================================================================================
 */

#include <fcntl.h>
#include <regex.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

#define SYSFS_RAPL_DIR "/sys/devices/virtual/powercap/intel-rapl"
#define MAX_PKGS 8

static int npkgs = 1;
static char *pkg_energy_uj[MAX_PKGS];
static char *dram_energy_uj[MAX_PKGS];
static char *pkg_energy_max[MAX_PKGS];
static char *dram_energy_max[MAX_PKGS];

static const char *readstr(const char *fn) {
  char buf[80];
  int fd;
  int rc;

  bzero(buf, sizeof(buf));

  fd = open(fn, O_RDONLY);
  if (fd < 0)
    return NULL;
  rc = read(fd, buf, sizeof(buf));
  if (rc < 0)
    buf[0] = 0;
  buf[sizeof(buf) - 1] = 0;
  close(fd);

  if ((strlen(buf) > 0) && (buf[strlen(buf) - 1] == '\n'))
    buf[strlen(buf) - 1] = 0;

  return strdup(buf);
}

int rapl_getnpkgs() { return npkgs; }

int rapl_init() {
  char fn[256];
  int rc;
  regex_t re;
  regmatch_t pm[3];
  int active_packages = 0;

  rc = regcomp(&re, "^package-([0-9]+)$", REG_EXTENDED);
  if (rc != 0) {
    fprintf(stderr, "regcomp() failed\n");
    return -1;
  }

  for (int i = 0; i < MAX_PKGS; i++) {
    snprintf(fn, sizeof(fn), "%s/intel-rapl:%d/name", SYSFS_RAPL_DIR, i);
    if (access(fn, R_OK) == 0) {
      const char *s = readstr(fn);
      rc = regexec(&re, s, 3, pm, 0);
      if (rc == 0) {
        int pkgid = atoi(s + pm[1].rm_so);
        snprintf(fn, sizeof(fn), "%s/intel-rapl:%d/energy_uj", SYSFS_RAPL_DIR,
                 i);
        pkg_energy_uj[pkgid] = strdup(fn);

        snprintf(fn, sizeof(fn), "%s/intel-rapl:%d/max_energy_range_uj",
                 SYSFS_RAPL_DIR, i);
        pkg_energy_max[pkgid] = strdup(fn);

        snprintf(fn, sizeof(fn), "%s/intel-rapl:%d/intel-rapl:%d:0/energy_uj",
                 SYSFS_RAPL_DIR, i, i);
        dram_energy_uj[pkgid] = strdup(fn);

        snprintf(fn, sizeof(fn),
                 "%s/intel-rapl:%d/intel-rapl:%d:0/max_energy_range_uj",
                 SYSFS_RAPL_DIR, i, i);
        dram_energy_max[pkgid] = strdup(fn);
        active_packages++;
      }
    } else {
      pkg_energy_uj[i] = NULL;
      dram_energy_uj[i] = NULL;
      pkg_energy_max[i] = NULL;
      dram_energy_max[i] = NULL;
    }
  }
  return active_packages;
}

uint64_t check_max(int pkgid, char *domain) {
  char *fn;
  uint64_t ret = 0;

  if (pkgid < 0 || pkgid >= MAX_PKGS)
    return 1;

  if (strcmp(domain, "pkg") == 0)
    fn = pkg_energy_max[pkgid];
  else if (strcmp(domain, "dram") == 0)
    fn = dram_energy_max[pkgid];

  if (!fn)
    return 2;

  const char *s = readstr(fn);
  if (!s)
    return 3;
  ret = atoll(s);
  return ret;
}

uint64_t rapl_readenergy_uj(int pkgid, char *domain) {
  char *fn;
  uint64_t ret = 0;

  if (pkgid < 0 || pkgid >= MAX_PKGS)
    return 1;

  if (strcmp(domain, "pkg") == 0)
    fn = pkg_energy_uj[pkgid];
  else if (strcmp(domain, "dram") == 0)
    fn = dram_energy_uj[pkgid];

  if (!fn)
    return 2;

  const char *s = readstr(fn);
  if (!s)
    return 3;
  ret = atoll(s);
  return ret;
}

static double gettime(void) {
  struct timeval tv;
  gettimeofday(&tv, 0);
  return (double)tv.tv_sec + (double)tv.tv_usec * 1e-6;
}

static void analyzeupdates(double *t, uint64_t *e, int n) {
  double prev_t = 0.0;
  double mean = 0.0;
  double prev_delta = 0.0;

  for (int i = 0; i < n; i++) {
    if (e[i] > 0) {
      if (prev_t == 0.0) {
        prev_t = t[i];
      } else {
        double delta = t[i] - prev_t;
        prev_t = t[i];

        if (prev_delta == 0.0) {
          mean = delta;
        } else {
          mean = (delta + prev_delta) / 2.0;
          prev_delta = delta;
        }
      }
    }
  }
  printf("Update freq. [kHz]: %lf\n", (1.0 / mean) * 1e-3);
}