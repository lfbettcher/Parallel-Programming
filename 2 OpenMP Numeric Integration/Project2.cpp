#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>

#include <iostream>

// print debugging messages?
#ifndef DEBUG
#define DEBUG false
#endif

// setting the number of threads: 1, 2, 4, 8
#ifndef NUMT
#define NUMT 2
#endif

// setting the number of nodes (subdivisions), pick at least 8 (1, 2, 4, ...)
#ifndef NUMNODES
#define NUMNODES 10
#endif

// how many tries to discover the maximum performance:
#ifndef NUMTRIES
#define NUMTRIES 10
#endif

#define XMIN -1.
#define XMAX 1.
#define YMIN -1.
#define YMAX 1.

#define N 0.70

float Height(int, int);
float AreaFraction(int, int);

int main(int argc, char *argv[]) {
#ifndef _OPENMP
  fprintf(stderr, "No OpenMP support!\n");
  return 1;
#endif

  omp_set_num_threads(NUMT);

  // the area of a single full-sized tile:
  // (not all tiles are full-sized, though)
  float fullTileArea = (((XMAX - XMIN) / (float)(NUMNODES - 1)) *
                        ((YMAX - YMIN) / (float)(NUMNODES - 1)));

  double maxPerformance = 0.;  // must be declared outside the NUMTRIES loop
  double volume;               // must be declared outside the NUMTRIES loop

  // Look for maximum performance:
  for (int tries = 0; tries < NUMTRIES; tries++) {
    volume = 0.;
    double time0 = omp_get_wtime();

    // sum up the weighted heights into the variable "volume"
    // using an OpenMP for loop and a reduction:
#pragma omp parallel for collapse(2) default(none), shared(fullTileArea), reduction(+ : volume)
    for (int iv = 0; iv < NUMNODES; iv++) {
      for (int iu = 0; iu < NUMNODES; iu++) {
        float height = Height(iu, iv);
        float frac = AreaFraction(iu, iv);
        volume += (double)frac * height * fullTileArea;
      }
    }
    double time1 = omp_get_wtime();
    double megaHeightsPerSecond =
        (double)NUMNODES * NUMNODES / (time1 - time0) / 1000000.;
    if (megaHeightsPerSecond > maxPerformance)
      maxPerformance = megaHeightsPerSecond;
  }  // for ( # of timing tries )

  fprintf(stderr,
          "%2d threads : %8d NUMNODES ; megaHeightsComputed/sec = "
          "%6.2lf; VOLUME = %f\n",
          NUMT, NUMNODES, maxPerformance, volume * 2);

  // output result as csv
  std::cout << maxPerformance << ",";

  return 0;
}

// Evaluates the height at a given iu and iv
float Height(int iu, int iv) {  // iu,iv = 0 .. NUMNODES-1
  float x = -1. + 2. * (float)iu / (float)(NUMNODES - 1);  // -1. to +1.
  float y = -1. + 2. * (float)iv / (float)(NUMNODES - 1);  // -1. to +1.

  float xn = pow(fabs(x), (double)N);
  float yn = pow(fabs(y), (double)N);
  float r = 1. - xn - yn;
  if (r <= 0.) return 0.;
  float height = pow(r, 1. / (float)N);
  return height;
}

// Checks if tile is on edge or corner
// center = 1.
// edge = 0.5
// corner = 0.25
float AreaFraction(int iu, int iv) {
  int max = NUMNODES - 1;
  // if edge tile
  if (iu == 0 || iu == max || iv == 0 || iv == max) {
    int arr[] = {0, 0, max, max, 0};
    // if corner tile
    for (int i = 0; i < 4; i++) {
      if (iu == arr[i] && iv == arr[i + 1]) {
        return 0.25;  // corner
      }
    }
    return 0.5;  // edge
  }
  return 1.;  // center
}
