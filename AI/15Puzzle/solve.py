from check_solvability import is_solvable

def solve(puzzle, goal , heuristic):
    if not is_solvable(puzzle):
        return
