/*****************************************************************************************************
 * File:            voronoi_omp.c
 * Author:          Isaiah Hendrick
 * Project:         Master's Project - PYNQ Voronoi Acceleration
 * Organization:    Grand Valley State University
 * 
 * Description:     This program generates a 2D Voronoi diagram using OpenMP for CPU parallelism. It 
 *                  computes which of a set of seeds is closest to each point in a 2D grid. Each point 
 *                  in the grid is assigned the index of the closest seed. Make sure to set the 
 *                  OMP_NUM_THREADS environment variable to the desired number of cores before running.
 * 
 * Compilation:     gcc voronoi_omp.c -o voronoi_omp -O3 -lm -fopenmp
 *****************************************************************************************************/

/* Libraries */
#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Macro definitions */
#define PROBLEM_SIZE    4096   // Size of the 2D grid (size x size)
#define NUM_SEEDS       4      // Number of seed points

/* Function declarations */
void calculate_voronoi(int, double *, double *);

int main(int argc, char *argv[]) {
    int size = PROBLEM_SIZE;

    // Dynamically allocate 2D array for Voronoi region assignments
    double(*area)[size] = calloc(size, sizeof *area);

    // Declare seed coordinate
    double seedVals[NUM_SEEDS][2] = {
        {0, 0}, 
        {0, size - 1}, 
        {size - 1, size - 1}, 
        {size - 1, 0}
    };

    // Flatten seed array for use in calculations
    double *seeds = (double *)malloc(sizeof(seedVals));
    memcpy(seeds, seedVals, sizeof(seedVals));

    // Time the Voronoi calculation
    double start = omp_get_wtime();
    calculate_voronoi(size, (double *)seeds, (double *)area);
    double end = omp_get_wtime();

    // Output results
    printf("-------------------------------------------\n");
    printf("| PARALLEL C (CPU) VORONOI IMPLEMENTATION |\n");
    printf("-------------------------------------------\n");
    printf("Array size: %d\n", size);
    printf("[C] voronoi_omp.c: %f seconds\n", end - start);

    // Free allocated memory
    free(seeds);
    free(area);

    return 0;
}

/**
* calculate_voronoi:
*   Computes which seed is closest to each point in the 2D grid.
*   Parallelized using OpenMP. Distance is calculated using Euclidean distance formula.
*
* Parameters:
*   - size: size of the grid (size x size)
*   - pSeeds: flat array of seed coordinates [x0, y0, x1, y1, ...]
*   - pArea: output array that holds the closest seed index for each point
*/
void calculate_voronoi(int size, double *pSeeds, double *pArea) {
    double closestSeed, closestDistance, dist;

    // Parallelized loop: collapse the two loops into a single iteration between threads
    #pragma omp parallel for collapse(2) private(closestSeed, closestDistance, dist)
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            closestSeed = -1;
            closestDistance = pow(size, 2); // Initialize to a large value

            // Check each seed to find the closest one
            for (int k = 0; k < NUM_SEEDS; k++) {
                dist = sqrt(pow(pSeeds[k * 2 + 0] - i, 2) +
                            pow(pSeeds[k * 2 + 1] - j, 2));

                // Update closest seed index
                if (dist < closestDistance) {
                    closestDistance = dist;
                    closestSeed = k;
                }
            }

            // Store the closest seed index
            pArea[i * size + j] = closestSeed;
        }
    }
}