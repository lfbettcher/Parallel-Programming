#include <math.h>
#include <omp.h>
#include <stdio.h>

#include "simd.p5.h"

#define NUMTRIES 20

// array size (1K to 8MM)
#ifndef ARRAYSIZE
#define ARRAYSIZE 10000
#endif

// number of threads
#ifndef NUMT
#define NUMT 1
#endif

// method to run
#ifndef METHOD
#define METHOD 0
#endif

#define NUM_ELEMENTS_PER_CORE ARRAYSIZE / NUMT

float A[ARRAYSIZE];
float B[ARRAYSIZE];
float C[ARRAYSIZE];

int main() {
#ifndef _OPENMP
  fprintf(stderr, "OpenMP is not supported here -- sorry.\n");
  return 1;
#endif

  // for-loop with no multicore or SIMD
  if (METHOD == 0) {
    double maxMegaMults = 0.;
    for (int t = 0; t < NUMTRIES; t++) {
      double time0 = omp_get_wtime();
      for (int i = 0; i < ARRAYSIZE; i++) {
        C[i] = A[i] * B[i];
      }
      double time1 = omp_get_wtime();

      double megaMults = (double)ARRAYSIZE / (time1 - time0) / 1000000.;
      if (megaMults > maxMegaMults) maxMegaMults = megaMults;
    }
    if (ARRAYSIZE == 1000) printf("%s,%d,", "no multicore or SIMD", 1);
    printf("%8.2lf,", maxMegaMults);
  }

  // Multicore+SIMD
  if (METHOD == 1) {
    // SIMD lecture slide 18
    omp_set_num_threads(NUMT);
    double maxMegaMults = 0.;
    for (int t = 0; t < NUMTRIES; t++) {
      double time0 = omp_get_wtime();
#pragma omp parallel
      {
        int first = omp_get_thread_num() * NUM_ELEMENTS_PER_CORE;
        SimdMul(&A[first], &B[first], &C[first], NUM_ELEMENTS_PER_CORE);
      }
      double time1 = omp_get_wtime();

      double megaMults = (double)ARRAYSIZE / (time1 - time0) / 1000000.;
      if (megaMults > maxMegaMults) maxMegaMults = megaMults;
    }
    if (ARRAYSIZE == 1000) printf("%s,%d,", "Multicore+SIMD", NUMT);
    printf("%8.2lf,", maxMegaMults);
  }

  // Multicore
  if (METHOD == 2) {
    omp_set_num_threads(NUMT);
    double maxMegaMults = 0.;
    for (int t = 0; t < NUMTRIES; t++) {
      double time0 = omp_get_wtime();

#pragma omp parallel for
      for (int i = 0; i < ARRAYSIZE; i++) {
        C[i] = A[i] * B[i];
      }
      double time1 = omp_get_wtime();

      double megaMults = (double)ARRAYSIZE / (time1 - time0) / 1000000.;
      if (megaMults > maxMegaMults) maxMegaMults = megaMults;
    }
    if (ARRAYSIZE == 1000) printf("%s,%d,", "Multicore", NUMT);
    printf("%8.2lf,", maxMegaMults);
  }

  return 0;
}
