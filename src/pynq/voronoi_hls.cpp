/**********************************************************************************************
 * File:            voronoi_hls.c
 * Author:          Isaiah Hendrick
 * Project:         Master's Project - PYNQ Voronoi Acceleration
 * Organization:    Grand Valley State University
 * 
 * Description:     
 **********************************************************************************************/

/* Libraries */
#include <hls_math.h>

/* Macro definitions */
#define NUM_SEEDS 4
#define SEED_COL_SIZE 2

void calculate_voronoi(int size, float* pArea) {
    #pragma HLS INTERFACE m_axi port=pArea offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=pArea bundle=control
    #pragma HLS INTERFACE s_axilite port=size bundle=control
    #pragma HLS INTERFACE s_axilite port=return bundle=control

    // Hardcode initialization of the seeds array
	float seeds[4][2];
	seeds[0][0] = 0;
	seeds[0][1] = 0;
	seeds[1][0] = 0;
	seeds[1][1] = size - 1;
	seeds[2][0] = size - 1;
	seeds[2][1] = size - 1;
	seeds[3][0] = size - 1;
	seeds[3][1] = 0;

    loop: for (int idx = 0; idx < size * size; idx++) {
        #pragma HLS PIPELINE II=1
        int i = idx / size;
        int j = idx % size;

        float closest_distance = size * size;
        float closest_seed = -1;

        for (int k = 0; k < NUM_SEEDS; k++) {
            #pragma HLS UNROLL
            float dx = seeds[k][0] - i;
            float dy = seeds[k][1] - j;
            float dist = hls::sqrtf(dx * dx + dy * dy);

            if (dist < closest_distance) {
                closest_distance = dist;
                closest_seed = (float)k;
            }
        }

        pArea[idx] = closest_seed;
    }
}

