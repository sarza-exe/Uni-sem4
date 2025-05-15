import random
import math
from check_solvability import is_solvable

# Precompute move table for every possible blank-tile position in N x N puzzle
# move_table[0]=[4, 1] from top-left (0,0) we can move down to idx=4 or right to idx=1
def build_move_table(N):
    move_table = {}
    for idx in range(N * N):
        moves = []
        row, col = divmod(idx, N) # row = idx // N, col = idx % N.
        if row > 0:
            moves.append(idx - N)  # up
        if row < N - 1:
            moves.append(idx + N)  # down
        if col > 0:
            moves.append(idx - 1)  # left
        if col < N - 1:
            moves.append(idx + 1)  # right
        move_table[idx] = moves
    return move_table

# Pretty-print a flat tuple state
def pretty_print(state: tuple):
    N = int(math.sqrt(len(state)))
    print("" + "__" * (2*N))
    for i in range(N):
        row = state[i*N:(i+1)*N]
        for val in row:
            s = f" {val}" if val < 10 else str(val)
            if val == 0:
                s = "  "
            print(s.rjust(3), end=" ")
        print()

# Generate default goal state as tuple
def generate_default_goal(puzzle_size: int) -> tuple:
    n = puzzle_size * puzzle_size
    # values 1..n-1, then 0
    return tuple(list(range(1, n)) + [0])

# Generate random puzzle by shuffling tiles
def generate_puzzle(puzzle_size: int) -> tuple:
    goal = generate_default_goal(puzzle_size)
    tiles = list(goal[:-1])  # exclude blank
    while True:
        random.shuffle(tiles)
        state = tuple(tiles + [0])
        if is_solvable(state):
            return state

# Generate random puzzle by k random moves from goal
def generate_puzzle_by_random_moves(puzzle_size: int = 4, k: int = 30) -> tuple:
    N = puzzle_size
    move_table = build_move_table(N)
    state = list(generate_default_goal(N))
    blank_idx = len(state) - 1  # starts at end
    last_move = None
    reverse = {(-N, N), (N, -N), (-1, 1), (1, -1)}  # but we track offsets
    for _ in range(k):
        possible = move_table[blank_idx]
        # filter reverse of last move
        if last_move is not None:
            back = blank_idx - last_move
            choices = [idx for idx in possible if idx != back]
        else:
            choices = possible[:]
        nxt = random.choice(choices)
        # swap blank and tile
        state[blank_idx], state[nxt] = state[nxt], state[blank_idx]
        last_move = nxt - blank_idx
        blank_idx = nxt
    return tuple(state)
