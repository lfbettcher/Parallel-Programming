#!/bin/bash

# print header row
printf "YYYY-MM,Precipitation (in),Temperature (°F),Grain Height (in),Number of Deer,Number of Wolves\n" > results.csv

# compile and run program
g++ project3.cpp -o project3 -lm -fopenmp
./project3 >> results.csv

printf "\n" >> results.csv

