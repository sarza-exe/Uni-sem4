import numpy as np
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
    # find the pos of blank
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


def board_key(board: np.ndarray) -> bytes:
    # np.ndarray.tobytes() gives a unique flat byte representation
    return board.tobytes()

def get_path(board):
    path = []
    node = board
    while node is not None:
        path.append(node.board)
        node = node.parent
    # reverse path so it's from beginning
    return path[::-1]

def solve(puzzle: np.ndarray, goal: np.ndarray, heuristic):
    if not is_solvable(puzzle):
        print("Puzzle is not solvable")
        return

    if np.array_equal(puzzle, goal):
        print("The puzzle is already solved")
        return

    visited_states = 0

    # Initialize the closed list (visited cells)
    closed_list = set()

    # Initialize the start cell details
    board_details = Board(puzzle)
    board_details.f = 0
    board_details.g = 0
    board_details.parent = None

    # Initialize the open list (cells to be visited) with the start cell
    open_list = []
    heapq.heappush(open_list, board_details)

    # Initialize the open dictionary for checking if board is on it
    open_dict: dict[bytes, Board] = {}
    k0 = board_key(puzzle)
    open_dict[k0] = board_details

    # key of goal board for comparing with others quicker
    goal_key = board_key(goal)

    # Main loop of A* search algorithm
    while len(open_list) > 0:
        current = heapq.heappop(open_list)
        visited_states += 1
        # print(len(open_list) , end=", ")
        #pretty_print(current.board)

        key_curr = board_key(current.board)
        open_dict.pop(key_curr, None)
        closed_list.add(key_curr)

        for neighbour in get_neighbours(current.board):
            key = board_key(neighbour)
            if key == goal_key:
                # print("The puzzle is solved")
                last_board = Board(neighbour)
                last_board.parent = current
                return get_path(last_board), visited_states

            # complexity of searching in set() is O(1)
            if key not in closed_list:
                # skip the neighbour that is a parent of current board
                if current.parent is not None and np.array_equal(neighbour, current.parent.board):
                    continue
                #print("b", end='')

                # Calculate the new f, g, and h values
                g_new = current.g + 1
                h_new = heuristic(neighbour, goal)
                f_new = g_new + h_new

                #If the cell is not in the open list or the new f value is smaller
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

        # if len(open_list) == 0:
        #     print(get_path(current))

    print("FAILED")