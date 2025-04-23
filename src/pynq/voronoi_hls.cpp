/**********************************************************************************************
 * File:            voronoi_hls.c
 * Author:          Isaiah Hendrick
 * Project:         Master's Project - PYNQ Voronoi Acceleration
 * Organization:    Grand Valley State University
 * 
 * Description:     This function implements a hardware-accelerated Voronoi diagram generator to be synthesized 
 *                  using Vitis HLS and deployed on the PYNQ-Z1 FPGA. It assigns each coordinate in a 2D grid 
 *                  to the nearest seed point (hardcoded to 4 corners) using Euclidean distance.
 * 
 *                  The function is designed to be synthesized as an AXI4-Lite peripheral and accessed via DMA.
 *                  The entire output grid is stored in external memory and accessed via an AXI master interface.
 **********************************************************************************************/

/* Libraries */
#include <hls_math.h>       // HLS-optimized math functions

/* Macro definitions */
#define NUM_SEEDS 4         // Total number of seed points

/**
 * calculate_voronoi:
 *   Assigns each point in a 2D grid to the nearest seed using Euclidean distance.
 *
 * Parameters:
 *  - size:  The dimension of the 2D grid (size x size)
 *  - pArea: Output pointer to a 1D array storing the index of the nearest seed for each point
 *
 * HLS Interface Notes:
 *  - pArea is accessed using AXI4 master (gmem) interface for external memory transfer
 *  - AXI4-Lite control interface is used for setting parameters and function control
 */
void calculate_voronoi(int size, float* pArea) {
    #pragma HLS INTERFACE m_axi port=pArea offset=slave bundle=gmem     // Memory-mapped AXI master port for reading/writing data
    #pragma HLS INTERFACE s_axilite port=pArea bundle=control           // Control interface for pArea
    #pragma HLS INTERFACE s_axilite port=size bundle=control            // Control interface for size
    #pragma HLS INTERFACE s_axilite port=return bundle=control          // Required to allow function invocation via AXI4-Lite

    // Hardcode initialization of the seeds array
	float seeds[NUM_SEEDS][2];
	seeds[0][0] = 0;
	seeds[0][1] = 0;
	seeds[1][0] = 0;
	seeds[1][1] = size - 1;
	seeds[2][0] = size - 1;
	seeds[2][1] = size - 1;
	seeds[3][0] = size - 1;
	seeds[3][1] = 0;

    // Loop over each point in the grid (flattened into 1D)
    loop: for (int idx = 0; idx < size * size; idx++) {
        #pragma HLS PIPELINE II=1                           // Fully pipeline the loop with initiation interval of 1
        int i = idx / size;                                 // Row index
        int j = idx % size;                                 // Column index

        float closest_distance = size * size;               // Initialize with max possible distance
        float closest_seed = -1;

        // Loop over each seed to compute the distance
        for (int k = 0; k < NUM_SEEDS; k++) {
            #pragma HLS UNROLL                              // Unroll loop to compute all distances in parallel
            float dx = seeds[k][0] - i;
            float dy = seeds[k][1] - j;
            float dist = hls::sqrtf(dx * dx + dy * dy);     // Use HLS-optimized square root

            // Update closest seed index
            if (dist < closest_distance) {
                closest_distance = dist;
                closest_seed = (float)k;
            }
        }

        // Store the index of the closest seed in the output array
        pArea[idx] = closest_seed;
    }
}