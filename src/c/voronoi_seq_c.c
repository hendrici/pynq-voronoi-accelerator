// #include <math.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <time.h>

// #define PROBLEM_SIZE    8192
// #define NUM_SEEDS       4
// #define RAND_GEN_SEED   4

// void gen_seeds(int, double *, double *);
// void calculate_voronoi(int, double *, double *, double *);
// int gen_voronoi_gnu_graph(int, double *);

// int main(int argc, char *argv[]) {
//   int size = PROBLEM_SIZE;
//   double(*area)[size] = calloc(size, sizeof *area);

//   double seedVals[NUM_SEEDS][2];
//   double seedWeights[NUM_SEEDS];

//   gen_seeds(size, (double *)seedVals, (double *)seedWeights);

//   double *seeds = (double *)malloc(sizeof(seedVals));
//   memcpy(seeds, seedVals, sizeof(seedVals));

//   clock_t start = clock();
//   calculate_voronoi(size, (double *)seeds, (double *)seedWeights, (double *)area);
//   clock_t end = clock();
//   double time_taken = (double)(end - start) / CLOCKS_PER_SEC;

//   printf("---------------------------------------\n");
//   printf("| SEQUENTIAL C VORONOI IMPLEMENTATION |\n");
//   printf("---------------------------------------\n");
//   printf("Array size: %d\n", size);
//   printf("Time taken: %f s\n", time_taken);
  
//   int x = 0;
//   // x = gen_voronoi_gnu_graph(size, (double *)area);

//   free(seeds);
//   free(area);

//   return x;
// }


// void gen_seeds(int size, double *pSeeds, double *pSeedWeights) {
//   srand(RAND_GEN_SEED);
  
//   // pSeeds[0] = 0;
//   // pSeeds[1] = 0;
//   // pSeeds[2] = size - 1;
//   // pSeeds[3] = 0;
//   // pSeeds[4] = 0;
//   // pSeeds[5] = size - 1;
//   // pSeeds[6] = size - 1;
//   // pSeeds[7] = size - 1;

//   // for (int i = 0; i < NUM_SEEDS; i++) {
//   //   pSeedWeights[i] = 1.0 / NUM_SEEDS;
//   // }

//   for (int i = 0; i < NUM_SEEDS; i++) {
//     pSeeds[i * 2 + 0] = rand() % size;
//     pSeeds[i * 2 + 1] = rand() % size;
//     pSeedWeights[i] = 1.0 / NUM_SEEDS;
//   }

//   double seedSum = 0;

//   // for (int i = 0; i < NUM_SEEDS; i++) {
//   //   pSeeds[i * 2 + 0] = rand() % size;
//   //   pSeeds[i * 2 + 1] = rand() % size;
//   //   pSeedWeights[i] = (double)rand() / RAND_MAX;
//   //   seedSum += pSeedWeights[i];
//   // }

//   // for (int i = 0; i < NUM_SEEDS; i++) {
//   //   pSeedWeights[i] /= seedSum;
//   //   printf("Seed %d: (%f, %f) - Weight: %f\n", i, pSeeds[i * 2 + 0], pSeeds[i * 2 + 1], pSeedWeights[i]);
//   // }
// }


// void calculate_voronoi(int size, double *pSeeds, double *pSeedWeights, double *pArea) {
//   double closestSeed, closestDistance, dist;

//   for (int i = 0; i < size; i++) {
//     for (int j = 0; j < size; j++) {
//       closestSeed = -1;
//       closestDistance = pow(size, 2);

//       for (int k = 0; k < NUM_SEEDS; k++) {
//         dist = sqrt(pow(pSeeds[k * 2 + 0] - i, 2) + pow(pSeeds[k * 2 + 1] - j, 2)) * pSeedWeights[k];
        
//         if (dist < closestDistance) {
//           closestDistance = dist;
//           closestSeed = k;
//         }
//       }

//       pArea[i * size + j] = closestSeed;
//     }
//   }
// }


// int gen_voronoi_gnu_graph(int size, double *pArea) {
//   char const *fileName = "../data/voronoi_data.txt";
//   FILE *pipeForGNUPlot = NULL;
//   FILE *file = fopen(fileName, "w");

//   if (file == NULL) {
//     printf("Could not open data file.\n");
//     return 1;
//   }

//   for (int i = 0; i < size; i++) {
//     for (int j = 0; j < size; j++) {
//       fprintf(file, "%d %d %d\n", i, j, (int)pArea[i * size + j]);
//     }
//   }

//   fclose(file);

//   pipeForGNUPlot = popen("gnuplot -p", "w");

//   if (pipeForGNUPlot == NULL) {
//     perror("popen failed.\n");
//     return 1;
//   }

//   fprintf(pipeForGNUPlot,
//             "set palette defined (0 \"blue\", 1 \"green\", 2 \"yellow\", 3 "
//             "\"red\")\n");
//   fprintf(pipeForGNUPlot,
//             "plot '../data/voronoi_data.txt' using 1:2:3 with points pt 7 ps 1.5 "
//             "palette\n");

//   pclose(pipeForGNUPlot);
  
//   return 0;
// }
/**********************************************************************************************
 * File:            voronoi_seq_c.c
 * Author:          Isaiah Hendrick
 * Project:         Master's Project - PYNQ Voronoi Acceleration
 * Organization:    Grand Valley State University
 * 
 * Description:     Sequential implementation of a 2D Voronoi diagram in C.
 *                  Each point in a 2D grid is assigned to the closest seed using 
 *                  Euclidean distance, adjusted by a weight for each seed.
 *
 *                  Seed coordinates and weights are randomly generated. Optionally, the 
 *                  result can be visualized using Gnuplot through output to a text file.
 *
 * Compilation:     
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
#define RAND_GEN_SEED   4      // RNG seed for reproducibility

/* Function declarations */
void gen_seeds(int, double *, double *);
void calculate_voronoi(int, double *, double *, double *);
int gen_voronoi_gnu_graph(int, double *);

int main(int argc, char *argv[]) {
    int size = PROBLEM_SIZE;

    // Dynamically allocate 2D area for storing Voronoi results
    double(*area)[size] = calloc(size, sizeof *area);

    // Seed coordinates and weights
    double seedVals[NUM_SEEDS][2];
    double seedWeights[NUM_SEEDS];

    // Generate seed positions and weights
    gen_seeds(size, (double *)seedVals, (double *)seedWeights);

    // Flatten the seed array for convenience in processing
    double *seeds = (double *)malloc(sizeof(seedVals));
    memcpy(seeds, seedVals, sizeof(seedVals));

    // Start timing
    clock_t start = clock();
    calculate_voronoi(size, seeds, seedWeights, (double *)area);
    clock_t end = clock();
    double time_taken = (double)(end - start) / CLOCKS_PER_SEC;

    // Output result summary
    printf("---------------------------------------\n");
    printf("| SEQUENTIAL C VORONOI IMPLEMENTATION |\n");
    printf("---------------------------------------\n");
    printf("Array size: %d\n", size);
    printf("[C] voronoi_seq_c.c: %.3f seconds\n", time_taken);

    // Optional: generate and plot using Gnuplot
    // int x = gen_voronoi_gnu_graph(size, (double *)area);

    // Clean up
    free(seeds);
    free(area);

    return 0;
}


/**
* gen_seeds:
*   Initializes seed coordinates and (uniform) weights.
*   Outputs flat array of [x0, y0, x1, y1, ...] in `pSeeds`
*   and a weight per seed in `pSeedWeights`.
*/
void gen_seeds(int size, double *pSeeds, double *pSeedWeights) {
    srand(RAND_GEN_SEED);  // Reproducible randomness

    for (int i = 0; i < NUM_SEEDS; i++) {
        pSeeds[i * 2 + 0] = rand() % size;  // x-coordinate
        pSeeds[i * 2 + 1] = rand() % size;  // y-coordinate
        pSeedWeights[i] = 1.0 / NUM_SEEDS;  // Uniform weights
    }

    // Optional: use variable weights (commented out)
    /*
    double seedSum = 0;
    for (int i = 0; i < NUM_SEEDS; i++) {
        pSeeds[i * 2 + 0] = rand() % size;
        pSeeds[i * 2 + 1] = rand() % size;
        pSeedWeights[i] = (double)rand() / RAND_MAX;
        seedSum += pSeedWeights[i];
    }

    for (int i = 0; i < NUM_SEEDS; i++) {
        pSeedWeights[i] /= seedSum;
        printf("Seed %d: (%f, %f) - Weight: %f\n", i,
              pSeeds[i * 2 + 0], pSeeds[i * 2 + 1], pSeedWeights[i]);
    }
    */
}


/**
* calculate_voronoi:
*   Iterates over each point in the grid and assigns it to the closest seed.
*   Uses weighted Euclidean distance: distance * seed weight.
*/
void calculate_voronoi(int size, double *pSeeds, double *pSeedWeights, double *pArea) {
    double closestSeed, closestDistance, dist;

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            closestSeed = -1;
            closestDistance = pow(size, 2);  // Max distance init

            for (int k = 0; k < NUM_SEEDS; k++) {
                // Weighted Euclidean distance to seed
                dist = sqrt(pow(pSeeds[k * 2 + 0] - i, 2) +
                            pow(pSeeds[k * 2 + 1] - j, 2)) * pSeedWeights[k];

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


/**
* gen_voronoi_gnu_graph:
*   Optional visualization function that outputs Voronoi data to a text file
*   and uses Gnuplot to display the results.
*/
int gen_voronoi_gnu_graph(int size, double *pArea) {
    const char *fileName = "../data/voronoi_data.txt";
    FILE *pipeForGNUPlot = NULL;
    FILE *file = fopen(fileName, "w");

    if (file == NULL) {
        printf("Could not open data file.\n");
        return 1;
    }

    // Write Voronoi index data to file for Gnuplot
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            fprintf(file, "%d %d %d\n", i, j, (int)pArea[i * size + j]);
        }
    }

    fclose(file);

    // Open Gnuplot for live plotting
    pipeForGNUPlot = popen("gnuplot -p", "w");
    if (pipeForGNUPlot == NULL) {
        perror("popen failed.\n");
        return 1;
    }

    // Gnuplot commands to style and render points
    fprintf(pipeForGNUPlot,
        "set palette defined (0 \"blue\", 1 \"green\", 2 \"yellow\", 3 \"red\")\n");
    fprintf(pipeForGNUPlot,
        "plot '../data/voronoi_data.txt' using 1:2:3 with points pt 7 ps 1.5 palette\n");

    pclose(pipeForGNUPlot);
    return 0;
}
