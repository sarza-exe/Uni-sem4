from puzzle import *
from heuristics import manhattan_heuristic
from solve import solve

N = 4
# funfact this 15 puzzle has 653'837'184'000 possible states (15!/2)

goal = generate_default_goal(N)
# puzzle0 = generate_puzzle(N)
# print(puzzle0)

puzzle1 = generate_puzzle_by_random_moves(4, 50)
print("Generated puzzle:")
print(puzzle1)
path, visited = solve(puzzle1, goal, manhattan_heuristic)
for p in path:
    pretty_print(p)
print("Visited states: ", visited)
print("The path to solve the puzzle has ", len(path), " nodes")

# Save solution path to path.txt
filepath = "path.txt"
with open(filepath, "w") as f:
    for state in path:
        # state is a flat tuple like (1,2,3,...,0)
        line = ",".join(str(x) for x in state)
        f.write(line + "\n")

import sys, os
os.system(f"{sys.executable} display.py {filepath}")