#include <float.h>
#include <math.h>
#include <omp.h>
#include <stdio.h>

#include <tuple>

#define SIZE 16384
#define NUMTRIES 20

float A[SIZE];
float B[SIZE];
float C[SIZE];

std::tuple<double, double> process(int);

int main() {
#ifndef _OPENMP
  fprintf(stderr, "OpenMP is not supported here -- sorry.\n");
  return 1;
#endif

  double maxMegaMults1, minTime1, maxMegaMults4, minTime4;
  std::tie(maxMegaMults1, minTime1) = process(1);
  std::tie(maxMegaMults4, minTime4) = process(4);
  double speedUpTime = minTime1 / minTime4;
  double speedUp = maxMegaMults4 / maxMegaMults1;
  printf("Speedup with Time = %8.2lf\n", speedUpTime);
  printf("Speedup with Performance = %8.2lf\n", speedUp);
  double FpTime = (4. / 3.) * (1. - (1. / speedUpTime));
  double Fp = (4. / 3.) * (1. - (1. / speedUp));
  printf("Parallel Fraction with Time = %8.2lf\n", FpTime);
  printf("Parallel Fraction with Performance = %8.2lf\n", Fp);

  // note: %lf stands for "long float", which is how printf prints a "double"
  //        %d stands for "decimal integer", not "double"

  return 0;
}

std::tuple<double, double> process(int NUMT) {
  // inialize the arrays:
  for (int i = 0; i < SIZE; i++) {
    A[i] = 1.;
    B[i] = 2.;
  }

  omp_set_num_threads(NUMT);
  fprintf(stderr, "Using %d threads\n", NUMT);

  double maxMegaMults = 0.;
  double minTime = DBL_MAX;

  for (int t = 0; t < NUMTRIES; t++) {
    double time0 = omp_get_wtime();

#pragma omp parallel for
    for (int i = 0; i < SIZE; i++) {
      // Using OpenMP, pairwise multiply two arrays and store results
      C[i] = A[i] * B[i];
    }

    // Time the run. Convert the timing to "Mega-Multiplies per Second"
    double time1 = omp_get_wtime();
    double executionTime = time1 - time0;
    double megaMults = (double)SIZE / executionTime / 1000000.;
    if (executionTime < minTime) minTime = executionTime;
    if (megaMults > maxMegaMults) maxMegaMults = megaMults;
  }

  printf("Min Execution Time = %8.2lf microseconds\n", minTime * 1000000.);
  printf("Peak Performance = %8.2lf MegaMults/Sec\n", maxMegaMults);
  return {maxMegaMults, minTime};
}
