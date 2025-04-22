from check_solvability import is_solvable
import random

def pretty_print(state):
    print("- "*12)
    for row in state:
        for element in row :
            # print like a table
            if len(str(element)) == 1:
                print("  ",element," ", end="")
            else:
                print(" ",element," ", end="")
        print()

def generate_default_goal(puzzle_size):
    goal = []
    x = 0
    for i in range(puzzle_size):
        line = []
        for j in range(puzzle_size):
            x += 1
            if x == puzzle_size * puzzle_size : x = 0
            line.append(x)
        goal.append(line)
    return goal

def generate_puzzle(puzzle_size):
    # Get the goal so we can grab all tiles except the blank
    goal = generate_default_goal(puzzle_size)
    flat_tiles = [tile for row in goal for tile in row if tile != 0]

    while True:
        # Shuffle the non‑zero tiles
        random.shuffle(flat_tiles)

        # Rebuild the state row by row putting 0 explicitly in the bottom‑right corner
        state = []
        idx = 0
        for i in range(puzzle_size):
            row = []
            for j in range(puzzle_size):
                if i == puzzle_size - 1 and j == puzzle_size - 1:
                    row.append(0)
                else:
                    row.append(flat_tiles[idx])
                    idx += 1
            state.append(row)

        # Check if puzzle is solvable
        if is_solvable(state):
            return state