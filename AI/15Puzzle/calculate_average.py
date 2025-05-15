from puzzle import *
from heuristics import manhattan_heuristic, linear_manhattan_heuristic
from solve import solve

N = 4
goal = generate_default_goal(N)

heuristics = [linear_manhattan_heuristic, manhattan_heuristic]
reps = 10000

for heuristic in heuristics:
    count_visited = 0
    count_path = 0
    max_visited = 0
    max_path = 0
    for x in range(reps):
        puzzle = generate_puzzle_by_random_moves()
        path, visited = solve(puzzle, goal, heuristic)
        max_visited = max(max_visited, visited)
        max_path = max(max_path, len(path))
        count_visited += visited
        count_path += len(path)
    print(heuristic.__name__, " Average statistics")
    print("Visited states: ", count_visited/reps, " Path length: ", count_path/reps)
    print("Max visited states: ", max_visited, " Max path length: ", max_path)