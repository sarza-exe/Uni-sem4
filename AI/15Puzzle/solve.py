import heapq
import math
from check_solvability import is_solvable
from puzzle import build_move_table

class Board:
    # reserve space for explicitly declared data members
    __slots__ = ('state', 'parent', 'f', 'g')

    def __init__(self, state, g=0, f=0, parent=None):
        self.state = state     # tuple of ints length N*N
        self.g = g             # cost from start to this board
        self.f = f             # total estimated cost (g + h)
        self.parent = parent   # parent's Board

    def __lt__(self, other):
        # heapq will use this to compare two Boards
        return self.f < other.f

# Generate neighbors of a state (tuple) using the move table
def get_neighbors(state, move_table):
    zero_idx = state.index(0) # index of element equal to 0
    neighbors = []
    for idx in move_table[zero_idx]:
        # swap zero and tile at idx
        new_state = list(state)
        new_state[zero_idx], new_state[idx] = new_state[idx], new_state[zero_idx]
        neighbors.append(tuple(new_state))
    return neighbors

# Reconstruct the path from goal Board to start
def get_path(board):
    path = []
    node = board
    while node:
        path.append(node.state)
        node = node.parent
    # reverse path so it's from beginning
    return path[::-1]

# heuristic: function(state: tuple, goal: tuple) -> int
def solve(puzzle, goal, heuristic):
    if not is_solvable(puzzle):
        print("Puzzle is not solvable")
        return None, 0

    if puzzle == goal:
        print("The puzzle is already solved")
        return [puzzle], 0

    N = int(math.sqrt(len(puzzle)))
    move_table = build_move_table(N)

    # Open and closed sets
    open_list = []
    open_dict = {}
    closed_list = set()

    # Initialize the start cell details
    h0 = heuristic(puzzle, goal, N)
    start = Board(state=puzzle, g=0, f=h0, parent=None)
    heapq.heappush(open_list, start)
    open_dict[puzzle] = start

    # for statistics
    visited_states = 0

    # Main loop of A* search algorithm
    while open_list:
        current = heapq.heappop(open_list)
        visited_states += 1

        if current.state == goal:
            return get_path(current), visited_states

        # Remove from open_dict and add to closed_list
        open_dict.pop(current.state, None)
        closed_list.add(current.state)

        for neighbor in get_neighbors(current.state, move_table):


            # complexity of searching in set() is O(1)
            if neighbor in closed_list:
                continue

            # Calculate the new f, g, and h values
            g_new = current.g + 1
            h_new = heuristic(neighbor, goal, N)
            f_new = g_new + h_new

            # If the cell is not in the open list or the new f value is smaller
            existing = open_dict.get(neighbor)
            if existing is None or f_new < existing.f:
                new_board = Board(state=neighbor, g=g_new, f=f_new, parent=current)
                heapq.heappush(open_list, new_board)
                open_dict[neighbor] = new_board

    print("FAILED to find a solution")
    return None, visited_states
