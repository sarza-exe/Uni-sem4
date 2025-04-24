from puzzle import *
from heuristics import walking_distance_heuristic, manhattan_heuristic
from solve import solve

N = 4
# funfact this 15 puzzle has 653'837'184'000 possible states (15!/2)

goal = generate_default_goal(N)
# puzzle0 = generate_puzzle(N)
# pretty_print(puzzle0)

puzzle1 = generate_puzzle_by_random_moves()
path, visited = solve(puzzle1, goal, walking_distance_heuristic)
for p in path:
    pretty_print(p)
print("Visited stated: ", visited)
print("The path to solve the puzzle has ", len(path), " nodes")
