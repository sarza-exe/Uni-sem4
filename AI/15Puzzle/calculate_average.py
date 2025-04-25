from puzzle import *
from heuristics import walking_distance_heuristic, manhattan_heuristic, linear_conflict_heuristic, linear_manhattan_heuristic
from solve import solve

N = 4
goal = generate_default_goal(N)

heuristics = [linear_manhattan_heuristic, walking_distance_heuristic, manhattan_heuristic, linear_conflict_heuristic]
reps = 10000

for heuristic in heuristics:
    count_visited = 0
    count_path = 0
    for x in range(reps):
        puzzle = generate_puzzle_by_random_moves()
        path, visited = solve(puzzle, goal, walking_distance_heuristic)
        count_visited += visited
        count_path += len(path)
    print(heuristic.__name__, " Average statistics")
    print("Visited states: ", count_visited/reps, " Path length: ", count_path/reps)