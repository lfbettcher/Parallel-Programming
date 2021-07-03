#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>

// print debugging messages?
#ifndef DEBUG
#define DEBUG false
#endif

#define MIN_YEAR 2021
#define MAX_YEAR 2026
#define MIN_MONTH 0
#define MAX_MONTH 11

// The "state" of the system consists of the following global variables:
int NowYear;      // 2021 - 2026
int NowMonth;     // 0 - 11
float NowPrecip;  // inches of rain per month
float NowTemp;    // temperature this month in degrees Fahrenheit(°F)
float NowHeight;  // grain height in inches
int NowNumDeer;   // number of deer in the current population
int NowNumWolf;   // number of wolf in the current population

const float ONE_WOLF_TRAMPLES_GRAIN_PER_MONTH = 1.0;
const int ONE_WOLF_KILLS_DEER_PER_MONTH = 2;

const float GRAIN_GROWS_PER_MONTH = 15.0;  // inches
const float ONE_DEER_EATS_PER_MONTH = 1.0;

const float AVG_PRECIP_PER_MONTH = 7.0;  // average
const float AMP_PRECIP_PER_MONTH = 6.0;  // plus or minus
const float RANDOM_PRECIP = 2.0;         // plus or minus noise

const float AVG_TEMP = 60.0;     // average
const float AMP_TEMP = 20.0;     // plus or minus
const float RANDOM_TEMP = 10.0;  // plus or minus noise

const float MIDTEMP = 40.0;
const float MIDPRECIP = 10.0;

// function prototypes:
float SQR(float);
float Ranf(unsigned int *, float, float);
unsigned int TimeOfDaySeed();

void Watcher();
void Deer();
void Grain();
void Wolf();

unsigned int seed = TimeOfDaySeed();

int main(int argc, char *argv[]) {
#ifndef _OPENMP
  fprintf(stderr, "No OpenMP support!\n");
  return 1;
#endif

  /* 1. Setup the Now global variables */
  // starting date and time:
  NowMonth = MIN_MONTH;
  NowYear = MIN_YEAR;
  // starting state (feel free to change this if you want):
  NowNumDeer = 5;
  NowHeight = 10.;
  NowNumWolf = 2;

  /* 2. Calculate the current Environmental Parameters */
  // The temperature and precipitation are a function of the particular month:
  float ang = (30. * (float)NowMonth + 15.) * (M_PI / 180.);

  float temp = AVG_TEMP - AMP_TEMP * cos(ang);
  NowTemp = temp + Ranf(&seed, -RANDOM_TEMP, RANDOM_TEMP);

  float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin(ang);
  NowPrecip = precip + Ranf(&seed, -RANDOM_PRECIP, RANDOM_PRECIP);
  if (NowPrecip < 0.) NowPrecip = 0.;

  // For simplicity, a year consists of 12 months of 30 days each.
  // The first day of winter is considered to be January 1.
  // The temperature and precipitation follow cosine and sine wave patterns
  // with some randomness added.

  /* 3. Spawn threads */
  // start the threads with a parallel sections directive:
  omp_set_num_threads(4);  // same as # of sections
#pragma omp parallel sections default(none)
  {
#pragma omp section
    { Deer(); }

#pragma omp section
    { Grain(); }

#pragma omp section
    { Watcher(); }

#pragma omp section
    { Wolf(); }
  }
  // implied barrier -- all functions must return in order
  // to allow any of them to get past here

  return 0;
}

void Watcher() {
  while (NowYear <= MAX_YEAR) {
// DoneComputing barrier:
#pragma omp barrier
    // ...

// DoneAssigning barrier:
#pragma omp barrier

    // output result as csv
    printf("%d-%02d,%f,%f,%f,%d,%d\n", NowYear, NowMonth + 1, NowPrecip,
           NowTemp, NowHeight, NowNumDeer, NowNumWolf);

    if (NowMonth == MAX_MONTH) {
      NowYear++;
      NowMonth = MIN_MONTH;
    } else {
      NowMonth++;
    }
    // Calculate new Environmental Parameters
    // The temperature and precipitation are a function of the particular month:
    float ang = (30. * (float)NowMonth + 15.) * (M_PI / 180.);

    float temp = AVG_TEMP - AMP_TEMP * cos(ang);
    NowTemp = temp + Ranf(&seed, -RANDOM_TEMP, RANDOM_TEMP);

    float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin(ang);
    NowPrecip = precip + Ranf(&seed, -RANDOM_PRECIP, RANDOM_PRECIP);
    if (NowPrecip < 0.) NowPrecip = 0.;

      // For simplicity, a year consists of 12 months of 30 days each.
      // The first day of winter is considered to be January 1.
      // The temperature and precipitation follow cosine and sine wave patterns
      // with some randomness added.

// DonePrinting barrier:
#pragma omp barrier
    // ...
  }
}

void Deer() {
  while (NowYear <= MAX_YEAR) {
    // Using the Now state, compute into the Next variables.
    // compute a temporary next-value for this quantity
    // based on the current state of the simulation:

    // Number of deer:
    int nextNumDeer = NowNumDeer - NowNumWolf * ONE_WOLF_KILLS_DEER_PER_MONTH;
    int carryingCapacity = (int)(NowHeight);
    if (nextNumDeer < carryingCapacity - 2)
      nextNumDeer += 3;
    else if (nextNumDeer < carryingCapacity)
      nextNumDeer++;
    else if (nextNumDeer > carryingCapacity)
      nextNumDeer--;

    if (nextNumDeer < 0) nextNumDeer = 0;

// DoneComputing barrier:
#pragma omp barrier
    // Copy the Next state into the Now variables
    NowNumDeer = nextNumDeer;

// DoneAssigning barrier:
#pragma omp barrier
    // ...

// DonePrinting barrier:
#pragma omp barrier
    // ...
  }
}

void Grain() {
  while (NowYear <= MAX_YEAR) {
    // Using the Now state, compute into the Next variables.
    // compute a temporary next-value for this quantity
    // based on the current state of the simulation:

    float tempFactor = exp(-SQR((NowTemp - MIDTEMP) / 10.));
    float precipFactor = exp(-SQR((NowPrecip - MIDPRECIP) / 10.));

    float nextHeight = NowHeight;
    nextHeight += tempFactor * precipFactor * GRAIN_GROWS_PER_MONTH;
    nextHeight -= (float)NowNumDeer * ONE_DEER_EATS_PER_MONTH;
    nextHeight -= (float)NowNumWolf * ONE_WOLF_TRAMPLES_GRAIN_PER_MONTH;
    if (nextHeight < 0.) nextHeight = 0.;

// DoneComputing barrier:
#pragma omp barrier
    // Copy the Next state into the Now variables
    NowHeight = nextHeight;  // grain height in inches

// DoneAssigning barrier:
#pragma omp barrier
    // ...

// DonePrinting barrier:
#pragma omp barrier
    // ...
  }
}

void Wolf() {
  while (NowYear <= MAX_YEAR) {
    // Using the Now state, compute into the Next variables.
    // compute a temporary next-value for this quantity
    // based on the current state of the simulation:

    // Number of wolves:
    int nextNumWolf = NowNumWolf;
    int carryingCapacity = NowNumDeer;
    if (nextNumWolf < carryingCapacity)
      nextNumWolf++;
    else if (nextNumWolf > carryingCapacity)
      nextNumWolf--;

    if (nextNumWolf < 0) nextNumWolf = 0;

// DoneComputing barrier:
#pragma omp barrier
    // Copy the Next state into the Now variables
    NowNumWolf = nextNumWolf;

// DoneAssigning barrier:
#pragma omp barrier
    // ...

// DonePrinting barrier:
#pragma omp barrier
    // ...
  }
}

float SQR(float x) { return x * x; }

float Ranf(unsigned int *seedp, float low, float high) {
  float r = (float)rand_r(seedp);  // 0 - RAND_MAX
  return (low + r * (high - low) / (float)RAND_MAX);
}

unsigned int TimeOfDaySeed() {
  struct tm y2k = {0};
  y2k.tm_hour = 0;
  y2k.tm_min = 0;
  y2k.tm_sec = 0;
  y2k.tm_year = 100;
  y2k.tm_mon = 0;
  y2k.tm_mday = 1;

  time_t timer;
  time(&timer);
  double seconds = difftime(timer, mktime(&y2k));
  seed = (unsigned int)(1000. * seconds);  // milliseconds
  return seed;
}
