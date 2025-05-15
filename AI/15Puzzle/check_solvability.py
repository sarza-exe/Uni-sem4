import math

import numpy as np
# A utility program to check if 15 puzzle is solvable

# A function to count inversions in given array
def get_inv_count(arr, size):
    inv_count = 0
    for i in range(size * size - 1):
        for j in range(i + 1, size * size):
            # count pairs(arr[i], arr[j]) such that
            # i < j and arr[i] > arr[j]
            if arr[j] and arr[i] and arr[i] > arr[j]:
                inv_count += 1

    return inv_count

# find Position of blank from bottom
def find_blank_pos(puzzle, size):
    # find the row index of the blank
    #blank_row = np.where(puzzle == 0)[0][0]
    blank_row = puzzle.index(0) // size
    # convert to distance from bottom
    return size - blank_row

# This function returns true if given instance of N*N - 1 puzzle is solvable
def is_solvable(puzzle):
    size = int(math.sqrt(len(puzzle)))
    # Count inversions in given puzzle
    inv_count = get_inv_count(puzzle, size)

    # If grid is odd, return true if inversion count is even.
    if size & 1:
        return inv_count % 2 == 0
    else:  # grid is even
        pos = find_blank_pos(puzzle, size)
        if pos & 1:
            return inv_count % 2 == 0
        else:
            return inv_count % 2 != 0


# puzzle1 = np.array([
#     [3, 9, 1, 15, ],
#     [14, 11, 4, 6, ],
#     [13, 5, 10, 12, ],  # Value 0 is used for blank
#     [2, 7, 8, 0, ], ])
#
# print("Solvable") if is_solvable(puzzle1) else print("Not Solvable")