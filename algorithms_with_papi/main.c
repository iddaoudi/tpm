/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  Task-based algorithms for power management
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

#include <papi.h>
#include "include/common.h"
#include "include/utils.h"
#include <pthread.h>

int tpm_allocate_tile(int M, tpm_desc **desc, int B) {
  int MT = M / B;
  *desc = (tpm_desc *)malloc(sizeof(tpm_desc));
  if (*desc == NULL) {
    printf("Tile allocation failed.\n");
    return 1;
  }
  **desc = tpm_matrix_desc_init(B, MT * B, B * B, MT * B);
  int info = tpm_matrix_desc_alloc(*desc);
  assert(!info);
  return 0;
}

int main(int argc, char *argv[]) {
  NTH = atoi(getenv("OMP_NUM_THREADS"));

  /* Command line arguments parsing */
  int arguments;
  char algorithm[16];
  struct option long_options[] = {{"Algorithm", required_argument, NULL, 'a'},
                                  {"Matrix size", required_argument, NULL, 'm'},
                                  {"Tile size", required_argument, NULL, 'b'},
                                  {NULL, no_argument, NULL, 0}};

  if (argc < 2) {
    printf("Missing arguments. Aborting...\n");
    exit(EXIT_FAILURE);
  }

  while ((arguments =
              getopt_long(argc, argv, "a:m:b:h:", long_options, NULL)) != -1) {
    if (optind > 2) {
      switch (arguments) {
      case 'a':
        if (optarg)
          strcpy(algorithm, optarg);
        break;
      case 'm':
        if (optarg)
          MSIZE = atoi(optarg);
        break;
      case 'b':
        if (optarg)
          BSIZE = atoi(optarg);
        break;
      case 'h':
        printf("HELP\n");
        exit(EXIT_FAILURE);
      case '?':
        printf("Invalid arguments. Aborting...\n");
        exit(EXIT_FAILURE);
      }
    }
  }

  if (MSIZE % BSIZE != 0) {
    printf("Tile size does not divide the matrix size. Aborting...\n");
    exit(EXIT_FAILURE);
  }
 
  PAPI_library_init(PAPI_VER_CURRENT);
  if (PAPI_thread_init(pthread_self) != PAPI_OK) {
      printf("PAPI_thread_init problem\n");
      exit(1);
  }
  /* Launch algorithms */
  double time_start, time_finish;
  if (strcmp(algorithm, "sparselu")) {
    /* Dense matrix memory allocation, creation and making it Hermitian positive
     */
    tpm_desc *A = NULL;
    double *ptr = NULL;
    int error = posix_memalign((void **)&ptr, getpagesize(),
                               MSIZE * MSIZE * sizeof(double));
    if (error) {
      printf("Problem allocating contiguous memory.\n");
      exit(EXIT_FAILURE);
    }
    tpm_matrix_desc_create(&A, ptr, BSIZE, MSIZE * MSIZE, BSIZE * BSIZE, MSIZE);
    
    if (!strcmp(algorithm, "cholesky")) {
      tpm_hermitian_positive_generator(*A);
      time_start = omp_get_wtime();
#pragma omp parallel
#pragma omp master
      { cholesky(*A); }
      //{ cholesky(*A, eventset, events, values, task_count); }
      time_finish = omp_get_wtime();
    } else if (!strcmp(algorithm, "qr")) {
      tpm_hermitian_positive_generator(*A);
      /* Workspace allocation for QR */
      tpm_desc *S = NULL;
      int ret = tpm_allocate_tile(MSIZE, &S, BSIZE);
      assert(ret == 0);
      time_start = omp_get_wtime();
#pragma omp parallel
#pragma omp master
      { qr(*A, *S); }
      time_finish = omp_get_wtime();
      free(S->matrix);
      tpm_matrix_desc_destroy(&S);
    } else if (!strcmp(algorithm, "lu")) {
      tpm_hermitian_positive_generator(*A);
      time_start = omp_get_wtime();
#pragma omp parallel
#pragma omp master
      { lu(*A); }
      time_finish = omp_get_wtime();
    }
    free(A->matrix);
    tpm_matrix_desc_destroy(&A);
  } else {
    /* Sparse matrix memory allocation and creation */
    double **M;
#pragma omp parallel
#pragma omp master
    tpm_sparse_allocate(&M, MSIZE, BSIZE);

    time_start = omp_get_wtime();
    sparselu(M, MSIZE, BSIZE);
    time_finish = omp_get_wtime();
    free(M);
  }
  if (TPM_TRACE_NO_OMPT)
    tpm_upstream_finalize();
  printf("%f\n", time_finish - time_start);
}
