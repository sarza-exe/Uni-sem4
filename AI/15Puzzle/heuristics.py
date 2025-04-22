
# NO. misplaced tiles
def hamming_heuristic(puzzle, goal):
    size = len(puzzle)
    misplaced = 0

    for i in range(size):
        for j in range(size):
            if puzzle[i][j] != 0 and puzzle[i][j] != goal[i][j]:
                misplaced += 1

    return misplaced

# The sum of the distances from the tiles to their goal positions
def manhattan_heuristic(puzzle, goal):
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