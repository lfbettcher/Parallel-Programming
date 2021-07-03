#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <string.h>

#include "simd.p5.h"

#define NUMTRIES 20

// array size (1K to 8MM)
#ifndef SIZE
#define SIZE 10000
#endif

// method to run
#ifndef METHOD
#define METHOD 0
#endif

float A[SIZE];
float B[SIZE];
float C[SIZE];

void NonSimdMul(float *a, float *b, float *c, int len) {
  for (int i = 0; i < len; i++) {
    c[i] = a[i] * b[i];
  }
}

float NonSimdMulSum(float *a, float *b, int len) {
  float sum = 0.0;
  for (int i = 0; i < len; i++) {
    sum += a[i] * b[i];
  }
  return sum;
}

int main() {
#ifndef _OPENMP
  fprintf(stderr, "OpenMP is not supported here -- sorry.\n");
  return 1;
#endif

  double maxMegaMults = 0.;

  // NonSimdMul
  if (METHOD == 0) {
    maxMegaMults = 0.;
    for (int t = 0; t < NUMTRIES; t++) {
      double time0 = omp_get_wtime();
      NonSimdMul(A, B, C, SIZE);
      double time1 = omp_get_wtime();
      double megaMults = (double)SIZE / (time1 - time0) / 1000000.;
      if (megaMults > maxMegaMults) maxMegaMults = megaMults;
    }
    if (SIZE == 1000) printf("NonSimdMul,");
    printf("%8.2lf,", maxMegaMults);
  }

  // SimdMul
  if (METHOD == 1) {
    maxMegaMults = 0.;
    for (int t = 0; t < NUMTRIES; t++) {
      double time0 = omp_get_wtime();
      SimdMul(A, B, C, SIZE);
      double time1 = omp_get_wtime();
      double megaMults = (double)SIZE / (time1 - time0) / 1000000.;
      if (megaMults > maxMegaMults) maxMegaMults = megaMults;
    }
    if (SIZE == 1000) printf("SimdMul,");
    printf("%8.2lf,", maxMegaMults);
  }

  // NonSimdMulSum
  if (METHOD == 2) {
    maxMegaMults = 0.;
    for (int t = 0; t < NUMTRIES; t++) {
      double time0 = omp_get_wtime();
      NonSimdMulSum(A, B, SIZE);
      double time1 = omp_get_wtime();
      double megaMults = (double)SIZE / (time1 - time0) / 1000000.;
      if (megaMults > maxMegaMults) maxMegaMults = megaMults;
    }
    if (SIZE == 1000) printf("NonSimdMulSum,");
    printf("%8.2lf,", maxMegaMults);
  }

  // SimdMulSum
  if (METHOD == 3) {
    maxMegaMults = 0.;
    for (int t = 0; t < NUMTRIES; t++) {
      double time0 = omp_get_wtime();
      SimdMulSum(A, B, SIZE);
      double time1 = omp_get_wtime();
      double megaMults = (double)SIZE / (time1 - time0) / 1000000.;
      if (megaMults > maxMegaMults) maxMegaMults = megaMults;
    }
    if (SIZE == 1000) printf("SimdMulSum,");
    printf("%8.2lf,", maxMegaMults);
  }

  return 0;
}
