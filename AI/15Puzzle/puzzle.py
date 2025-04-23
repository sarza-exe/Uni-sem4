import numpy as np
from check_solvability import is_solvable

def pretty_print(state: np.ndarray):
    print("- "*12)
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