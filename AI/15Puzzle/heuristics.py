import math

# Heuristics working on flat tuple states for an N x N sliding puzzle

def manhattan_heuristic(state: tuple, goal: tuple, N: int) -> int:
    """
    Sum of Manhattan distances of each tile from its goal position.
    """
    # Build mapping from tile value to its goal index
    goal_pos = {tile: idx for idx, tile in enumerate(goal)}
    total = 0
    for idx, tile in enumerate(state):
        if tile == 0:
            continue
        gi = goal_pos[tile]
        row_s, col_s = divmod(idx, N)
        row_g, col_g = divmod(gi, N)
        total += abs(row_s - row_g) + abs(col_s - col_g)
    return total


def linear_conflict_heuristic(state: tuple, goal: tuple, N: int) -> int:
    """
    Adds 2 moves for each pair of tiles in the same row or column
    that are in the same goal line but in reversed order.
    """
    # Build mapping from tile value to its goal index
    goal_pos = {tile: idx for idx, tile in enumerate(goal)}
    conflict = 0
    # Row conflicts
    # in row values in goal_positions are in ascending order. this adds +2 to conflict if two tiles
    # are in correct row but reverse positions
    for r in range(N):
        max_val = -1
        for c in range(N):
            idx = r * N + c
            tile = state[idx]
            if tile != 0:
                gi = goal_pos[tile]
                gr, gc = divmod(gi, N)
                if gr == r:  # tile belongs in this row
                    if tile > max_val:
                        max_val = tile
                    else:
                        conflict += 2
    # Column conflicts
    for c in range(N):
        max_val = -1
        for r in range(N):
            idx = r * N + c
            tile = state[idx]
            if tile != 0:
                gi = goal_pos[tile]
                gr, gc = divmod(gi, N)
                if gc == c:  # tile belongs in this column
                    if tile > max_val:
                        max_val = tile
                    else:
                        conflict += 2
    return conflict


def linear_manhattan_heuristic(state: tuple, goal: tuple, N: int) -> int:
    """
    Manhattan distance plus linear conflict.
    """
    return manhattan_heuristic(state, goal, N) + linear_conflict_heuristic(state, goal, N)
