import numpy as np
from check_solvability import is_solvable
from puzzle import *
from heuristics import hamming_heuristic, manhattan_heuristic
from solve import solve

N = 4
puzzle2 = np.array([
    [3, 9, 1, 15, ],
    [14, 11, 4, 6, ],
    [13, 5, 10, 12, ],
    [2, 7, 8, 0, ], ]) # Value 0 is used for blank

if not is_solvable(puzzle2):
    print("Not ")
print("Solvable")

goal = generate_default_goal(N)
puzzle = generate_puzzle(N)
pretty_print(puzzle)


path = solve(puzzle, goal, manhattan_heuristic)
print("The path to solve the puzzle has ", len(path), " nodes")
for p in path:
    pretty_print(p)
# states_visited, path = solve(puzzle, goal, hamming_heuristic)
