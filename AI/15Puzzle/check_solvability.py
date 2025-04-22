# A utility program to check if 15 puzzle is solvable
N = 4

# A function to count inversions in given array
def get_inv_count(arr):
    arr1 = []
    for y in arr:
        for x in y:
            arr1.append(x)
    arr = arr1
    inv_count = 0
    for i in range(N * N - 1):
        for j in range(i + 1, N * N):
            # count pairs(arr[i], arr[j]) such that
            # i < j and arr[i] > arr[j]
            if arr[j] and arr[i] and arr[i] > arr[j]:
                inv_count += 1

    return inv_count

# find Position of blank from bottom
def find_blank_pos(puzzle):
    # start from bottom-right corner of matrix
    for i in range(N - 1, -1, -1):
        for j in range(N - 1, -1, -1):
            if puzzle[i][j] == 0:
                return N - i

# This function returns true if given instance of N*N - 1 puzzle is solvable
def is_solvable(puzzle):
    # Count inversions in given puzzle
    inv_count = get_inv_count(puzzle)

    # If grid is odd, return true if inversion count is even.
    if N & 1:
        return ~(inv_count & 1)
    else:  # grid is even
        pos = find_blank_pos(puzzle)
        if pos & 1:
            return ~(inv_count & 1)
        else:
            return inv_count & 1


# puzzle = [
#     [3, 9, 1, 15, ],
#     [14, 11, 4, 6, ],
#     [13, 5, 10, 12, ],  # Value 0 is used for empty space
#     [2, 7, 8, 0, ], ]
#
# print("Solvable") if is_solvable(puzzle) else print("Not Solvable")

# ctrl + / to comment