import numpy as np

# TODO other heuristic
def walking_distance_heuristic(puzzle: np.ndarray, goal: np.ndarray):
    # Create a grid to store the walking distances
    size = len(puzzle)
    distance_grid = [[0] * size for _ in range(size)]

    for row in range(size):
        for col in range(size):
            value = puzzle[row][col]
            if value != 0:
                target_row = (value - 1) // size
                target_col = (value - 1) % size
                distance_grid[row][col] = abs(row - target_row) + abs(col - target_col)

    # Calculate the walking distance
    walking_distance = 0
    for row in range(size):
        for col in range(size):
            walking_distance += distance_grid[row][col]

    return walking_distance

# NO. misplaced tiles
def hamming_heuristic(puzzle: np.ndarray, goal: np.ndarray):
    # mask of non-blank tiles that differ from goal
    misplaced = (puzzle != 0) & (puzzle != goal)
    return int(np.sum(misplaced))

# The sum of the distances from the tiles to their goal positions
def manhattan_heuristic(puzzle: np.ndarray, goal: np.ndarray):

    size = len(puzzle)
    # Build a map from tile value -> (goal_row, goal_col)
    goal_pos = {}
    for i in range(size):
        for j in range(size):
            goal_pos[goal[i][j]] = (i, j)

    # Sum Manhattan distances for each tile (skip the blank '0')
    total_dist = 0
    for i in range(size):
        for j in range(size):
            tile = puzzle[i][j]
            if tile != 0:
                gi, gj = goal_pos[tile]
                total_dist += abs(i - gi) + abs(j - gj)

    return total_dist

def linear_conflict_heuristic(puzzle: np.ndarray, goal: np.ndarray):
    conflict = 0
    size = len(puzzle)

    # Row conflicts
    for row in range(size):
        max_val = -1
        for col in range(size):
            value = puzzle[row][col]
            if value != 0 and (value - 1) // size == row:
                if value > max_val:
                    max_val = value
                else:
                    conflict += 2

    # Column conflicts
    for col in range(size):
        max_val = -1
        for row in range(size):
            value = puzzle[row][col]
            if value != 0 and (value - 1) % size == col:
                if value > max_val:
                    max_val = value
                else:
                    conflict += 2

    return conflict

def linear_manhattan_heuristic(puzzle: np.ndarray, goal: np.ndarray):
    return manhattan_heuristic(puzzle, goal) + linear_conflict_heuristic(puzzle, goal)