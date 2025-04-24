import numpy as np
import random
from check_solvability import is_solvable

def pretty_print(state: np.ndarray):
    print("__"*12)
    for row in state:
        for element in row :
            # print like a table
            if len(str(element)) == 1:
                print("  ",element," ", end="")
            else:
                print(" ",element," ", end="")
        print()

def generate_default_goal(puzzle_size) -> np.ndarray:
    n = puzzle_size * puzzle_size
    # make [1,2,...,N^2], then set the last element to 0
    goal = np.arange(1, n + 1, dtype=int)
    goal[-1] = 0
    return goal.reshape(puzzle_size, puzzle_size)

def generate_puzzle(puzzle_size) -> np.ndarray:
    # Get the goal so we can grab all tiles except the blank
    goal = generate_default_goal(puzzle_size)
    # get a copy of all non-zero tiles
    flat_tiles = goal.flatten()
    flat_tiles = flat_tiles[flat_tiles != 0].copy()

    while True:
        # shuffle in place
        np.random.shuffle(flat_tiles)
        # append the blank (0) and reshape back to N×N
        arr = np.concatenate([flat_tiles, [0]])
        state = arr.reshape(puzzle_size, puzzle_size)

        if is_solvable(state):
            return state


def generate_puzzle_by_random_moves(puzzle_size: int = 4, k: int = 30) -> np.ndarray:
    """
    Generate a random solvable sliding puzzle by applying k random moves
    to the goal state.
    k (int): Number of random backward moves (must be > 20).
    """
    if k <= 20:
        raise ValueError("Number of moves k must be greater than 20")

    # Start from the goal state
    puzzle = generate_default_goal(puzzle_size).copy()
    # Track the blank (empty) position
    blank_r, blank_c = puzzle_size - 1, puzzle_size - 1

    # Define possible move directions (row_change, col_change)
    moves = [(-1, 0), (1, 0), (0, -1), (0, 1)]
    # Reverse move lookup to avoid immediate backtracking
    reverse = {(-1, 0): (1, 0), (1, 0): (-1, 0), (0, -1): (0, 1), (0, 1): (0, -1)}
    last_move = None

    for _ in range(k):
        # Collect valid moves (inside bounds and not reversing last move)
        valid_moves = []
        for dr, dc in moves:
            nr, nc = blank_r + dr, blank_c + dc
            if 0 <= nr < puzzle_size and 0 <= nc < puzzle_size:
                if last_move is None or (dr, dc) != reverse[last_move]:
                    valid_moves.append((dr, dc))

        # Choose a random valid move
        dr, dc = random.choice(valid_moves)
        nr, nc = blank_r + dr, blank_c + dc

        # Swap the blank with the chosen tile
        puzzle[blank_r, blank_c], puzzle[nr, nc] = puzzle[nr, nc], puzzle[blank_r, blank_c]

        # Update blank position and last move
        blank_r, blank_c = nr, nc
        last_move = (dr, dc)

    return puzzle