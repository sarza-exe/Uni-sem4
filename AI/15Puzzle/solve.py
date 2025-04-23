import numpy as np
import math
import heapq
from check_solvability import is_solvable
from puzzle import pretty_print

class Board:
    def __init__(self, board: np.ndarray):
        self.board = board.copy()
    # Parent cell's board
        self.parent = None
    # Total cost of the cell (g + h)
        self.f = 0
    # Cost from start to this cell
        self.g = 0

    def __lt__(self, other: "Board") -> bool:
        # heapq will use this to compare two Boards
        return self.f < other.f

# return a list of all boards reachable in one move.
def get_neighbours(board: np.ndarray):
    size = board.shape[0]
    # find the blank’s (row, col)
    i, j = np.argwhere(board == 0)[0]

    neighbours = []
    # offsets for up, down, left, right
    for di, dj in [(-1, 0), (1, 0), (0, -1), (0, 1)]:
        ni, nj = i + di, j + dj
        # check bounds
        if 0 <= ni < size and 0 <= nj < size:
            new_board = board.copy()
            # swap blank with neighbour
            new_board[i, j], new_board[ni, nj] = new_board[ni, nj], new_board[i, j]
            neighbours.append(new_board)

    return neighbours



def is_in_closed(neighbour: np.ndarray, closed_list):
    return any(np.array_equal(neighbour, closed) for closed in closed_list)

def find_in_open(board: np.ndarray, lst) -> Board | None:
    return next(
        (node for node in lst if np.array_equal(node.board, board)),
        None
    )

def board_key(board: np.ndarray) -> bytes:
    # np.ndarray.tobytes() gives a unique flat byte representation
    return board.tobytes()

def get_path(board):
    path = []
    node = board
    while node is not None:
        path.append(node.board)
        node = node.parent
    return path[::-1]

def solve(puzzle: np.ndarray, goal: np.ndarray, heuristic):
    if not is_solvable(puzzle):
        print("Puzzle is not solvable")
        return

    if np.array_equal(puzzle, goal):
        print("The puzzle is already solved")
        return

    # Initialize the closed list (visited cells)
    closed_list = []
    visited = set()

    # Initialize the start cell details
    board_details = Board(puzzle)
    board_details.f = 0
    board_details.g = 0
    board_details.parent = None

    # Initialize the open list (cells to be visited) with the start cell
    open_list = []
    heapq.heappush(open_list, board_details) # TODO ???

    open_dict: dict[bytes, Board] = {}
    k0 = board_key(puzzle)
    open_dict[k0] = board_details

    found_dest = False

    # Main loop of A* search algorithm
    while len(open_list) > 0:
        # print(len(open_list) , end=', ')
        current = heapq.heappop(open_list)
        #pretty_print(current.board)
        #print("Current:\n", current.board)
        closed_list.append(current)

        key_curr = board_key(current.board)
        visited.add(key_curr)
        open_dict.pop(key_curr, None)

        for neighbour in get_neighbours(current.board):
            if np.array_equal(neighbour, goal):
                print("The puzzle is solved")
                found_dest = True
                last_board = Board(neighbour)
                last_board.parent = current
                return get_path(last_board)

            #print(len(closed_list) , end='')
            key = board_key(neighbour)
            if key not in visited:

                if current.parent is not None and np.array_equal(neighbour, current.parent.board):
                    continue
                #print("b", end='')
                # Calculate the new f, g, and h values
                g_new = current.g + 1
                h_new = heuristic(neighbour, goal)
                f_new = g_new + h_new

                # TODO If the cell is not in the open list or the new f value is smaller
                existing = open_dict.get(key)
                if existing is None or f_new < existing.f:

                    #print(g_new, " ", h_new, " ", f_new)
                    # Update the cell details
                    new_board = Board(neighbour)
                    new_board.f = f_new
                    new_board.g = g_new
                    new_board.parent = current

                    # Add the cell to the open list
                    heapq.heappush(open_list, new_board)

        if len(open_list) == 0:
            print(get_path(current))

    if not found_dest:
        print("FAILED")

# print("c", end='')
# existing = find_in_open(neighbour, open_list)
# print(len(open_list), end=' ')
# if existing is None or existing.f > f_new:



