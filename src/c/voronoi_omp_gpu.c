/**************************************************************************************************
 * File:            voronoi_omp_gpu.c
 * Author:          Isaiah Hendrick
 * Project:         Master's Project - PYNQ Voronoi Acceleration
 * Organization:    Grand Valley State University
 * 
 * Description:     Parallel Voronoi diagram generator using OpenMP Offloading for GPU.
 *                  Each point in a 2D grid is assigned the index of the nearest seed,
 *                  using Euclidean distance.
 *
 *                  This implementation uses OpenMP's `target teams` and `parallel for simd`
 *                  constructs to run the main computation on a target device such as a GPU.
 *
 * Compilation:     gcc -O2 voronoi_parallel_gpu.c -o voronoi -lm
 **************************************************************************************************/

/* Libraries */
#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Macro definitions */
#define SEED_ROW_SIZE 4
#define SEED_COL_SIZE 2

/* Function declarations */
void calculate_voronoi(int, double *, double *);

int main(int argc, char *argv[]) {
    int size = 4096;

    // Allocate memory for the Voronoi output grid
    double(*area)[size] = calloc(size, sizeof *area);

    // Define fixed seed locations (corners of the grid)
    double seed_vals[SEED_ROW_SIZE][SEED_COL_SIZE] = {
        {0, 0},
        {0, size - 1},
        {size - 1, size - 1},
        {size - 1, 0}
    };

    // Flatten seed array for device use
    double *seeds = (double *)malloc(sizeof(seed_vals));
    memcpy(seeds, seed_vals, sizeof(seed_vals));

    // Time the GPU computation
    double start = omp_get_wtime();
    calculate_voronoi(size, seeds, (double *)area);
    double end = omp_get_wtime();

    // Output performance results
    printf("-------------------------------------------\n");
    printf("| PARALLEL C (GPU) VORONOI IMPLEMENTATION |\n");
    printf("-------------------------------------------\n");
    printf("Array size: %d\n", size);
    printf("[C] voronoi_omp_gpu.c: %.3f seconds\n", end - start);

    // Clean up memory
    free(seeds);
    free(area);

    return 0;
}


/**
* calculate_voronoi:
*   GPU-offloaded function that assigns each point in the grid to
*   the closest seed using Euclidean distance.
*
*   Parameters:
*     - size: grid dimension (size x size)
*     - pSeeds: flattened seed array [x0, y0, x1, y1, ...]
*     - pArea: output array storing closest seed index for each point
*/
void calculate_voronoi(int size, double *pSeeds, double *pArea) {
    double closest_seed, closest_distance, dist;

    // Number of GPU teams (can be tuned)
    int num_blocks = omp_get_max_threads();

    // Offload loop to GPU using OpenMP target directives
    #pragma omp target teams num_teams(num_blocks) map(to: pSeeds[0:SEED_ROW_SIZE * SEED_COL_SIZE], size) map(from: pArea[0:size * size])
    #pragma omp distribute parallel for collapse(2) firstprivate(closest_seed, closest_distance, dist)
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            closest_seed = -1;
            closest_distance = pow(size, 2);  // Max possible distance

            // Compare current point to each seed
            for (int k = 0; k < SEED_ROW_SIZE; k++) {
                dist = sqrt(
                    pow(pSeeds[k * SEED_COL_SIZE + 0] - i, 2) +
                    pow(pSeeds[k * SEED_COL_SIZE + 1] - j, 2)
                );

                if (dist < closest_distance) {
                    closest_distance = dist;
                    closest_seed = k;
                }
            }

            // Assign the index of the closest seed
            pArea[i * size + j] = closest_seed;
        }
    }
}
