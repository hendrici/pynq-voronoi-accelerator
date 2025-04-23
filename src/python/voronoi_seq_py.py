# -----------------------------------------------------------------------------------------
# File:           voronoi_seq_py.py
# Author:         Isaiah Hendrick (Adapted from Dr. Christian Trefftz)
# Project:        Master's Project - PYNQ Voronoi Acceleration
# Organization:   Grand Valley State University
#
# Description:    Sequential implementation of a Voronoi diagram in Python. This program 
#                 computes which seed is closest to each coordinate in a 2D grid using 
#                 Euclidean distance. The result is a 2D array mapping each coordinate 
#                 to the index of its closest seed.
# -----------------------------------------------------------------------------------------

import numpy as np
import time

def calculate_voronoi(size, seeds):
    """
    Computes the Voronoi diagram for a grid of given size.

    Parameters:
    - size: int, the width and height of the 2D area
    - seeds: numpy array of shape (N, 2), the coordinates of seed points

    Returns:
    - area: numpy array of shape (size, size), each entry contains 
            the index of the closest seed to that point
    """
    # Initialize the result area with zeros
    area = np.zeros((size, size))

    # Iterate over each point in the grid
    for i in range(size):
        for j in range(size):
            closest_seed = -1
            closest_distance = size * size  # Initially, set max possible distance

            # Compute the distance to each seed
            for k in range(len(seeds)):
                point1 = np.array([float(seeds[k][0]), float(seeds[k][1])])  # Seed point
                point2 = np.array([float(i), float(j)])  # Current grid point
                temp = point1 - point2
                sum_sq = np.sum(np.square(temp))  # Squared distance
                dist = np.sqrt(sum_sq)  # Euclidean distance

                # Check if this seed is closer than the previous closest
                if dist < closest_distance:
                    closest_distance = dist
                    closest_seed = k

            # Store the index of the closest seed
            area[i][j] = closest_seed

    return area

if __name__ == "__main__":
    # Define problem size and seed values
    size = 4096
    seeds = np.array([[0, 0], [0, size-1], [size-1, size-1], [size-1, 0]])

    # start the Voronoi computation
    start = time.time()
    area = calculate_voronoi(size, seeds)
    end = time.time()
    elapsed = end - start

    # Display timing results
    print("--------------------------------------------")
    print("| SEQUENTIAL PYTHON VORONOI IMPLEMENTATION |")
    print("--------------------------------------------")
    print(f"Array size: {size}")
    print(f"[PY] voronoi_seq_py.py: {elapsed} seconds")