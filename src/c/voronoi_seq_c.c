/**********************************************************************************************
 * File:            voronoi_seq_c.c
 * Author:          Isaiah Hendrick
 * Project:         Master's Project - PYNQ Voronoi Acceleration
 * Organization:    Grand Valley State University
 * 
 * Description:     Sequential implementation of a 2D Voronoi diagram in C. Each point in a 2D 
 *                  grid is assigned to the closest seed using Euclidean distance.
 *
 * Compilation:     gcc voronoi.c -o voronoi -O3 -lm
 **********************************************************************************************/

/* Libraries */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Macro definitions */
#define PROBLEM_SIZE    4096   // Grid size (N x N)
#define NUM_SEEDS       4      // Number of seeds

/* Function declarations */
void calculate_voronoi(int, double *, double *);

int main(int argc, char *argv[]) {
    int size = PROBLEM_SIZE;

    // Dynamically allocate 2D area for storing Voronoi results
    double(*area)[size] = calloc(size, sizeof *area);

    // Seed coordinates
    double seedVals[NUM_SEEDS][2] = {
        {0, 0}, 
        {0, size - 1}, 
        {size - 1, size - 1}, 
        {size - 1, 0}
    };

    // Flatten the seed array for convenience in processing
    double *seeds = (double *)malloc(sizeof(seedVals));
    memcpy(seeds, seedVals, sizeof(seedVals));

    // Start timing
    clock_t start = clock();
    calculate_voronoi(size, (double *)seeds, (double *)area);
    clock_t end = clock();
    double time_taken = (double)(end - start) / CLOCKS_PER_SEC;

    // Output result summary
    printf("---------------------------------------\n");
    printf("| SEQUENTIAL C VORONOI IMPLEMENTATION |\n");
    printf("---------------------------------------\n");
    printf("Array size: %d\n", size);
    printf("[C] voronoi_seq_c.c: %f seconds\n", time_taken);

    // Clean up (free allocated mem)
    free(seeds);
    free(area);

    return 0;
}


/**
* calculate_voronoi:
*   Iterates over each point in the grid and assigns it to the closest seed.
*   Uses Euclidean distance to calculate distance between points.
*
*   Parameters:
*     - size: grid dimension (size x size)
*     - pSeeds: flattened seed array [x0, y0, x1, y1, ...]
*     - pArea: output array storing closest seed index for each point
*/
void calculate_voronoi(int size, double *pSeeds, double *pArea) {
    double closestSeed, closestDistance, dist;

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            closestSeed = -1;
            closestDistance = pow(size, 2);  // Max distance init

            for (int k = 0; k < NUM_SEEDS; k++) {
                // Weighted Euclidean distance to seed
                dist = sqrt(pow(pSeeds[k * 2 + 0] - i, 2) +
                            pow(pSeeds[k * 2 + 1] - j, 2));

                // Update closest seed index
                if (dist < closestDistance) {
                    closestDistance = dist;
                    closestSeed = k;
                }
            }

            // Store closest seed index
            pArea[i * size + j] = closestSeed;
        }
    }
}