#pragma once

// Original win/lose pattern code by Prof. Maciej Gębala (CC BY-NC 4.0)
// Modified for educational use

// Position class for 5x5 Tic-Tac-Toe variant

#include <vector>
#include <iostream>

const int win[28][4][2] = {
  { {0, 0}, {0, 1}, {0, 2}, {0, 3} },
  { {1, 0}, {1, 1}, {1, 2}, {1, 3} },
  { {2, 0}, {2, 1}, {2, 2}, {2, 3} },
  { {3, 0}, {3, 1}, {3, 2}, {3, 3} },
  { {4, 0}, {4, 1}, {4, 2}, {4, 3} },
  { {0, 1}, {0, 2}, {0, 3}, {0, 4} },
  { {1, 1}, {1, 2}, {1, 3}, {1, 4} },
  { {2, 1}, {2, 2}, {2, 3}, {2, 4} },
  { {3, 1}, {3, 2}, {3, 3}, {3, 4} },
  { {4, 1}, {4, 2}, {4, 3}, {4, 4} },
  { {0, 0}, {1, 0}, {2, 0}, {3, 0} },
  { {0, 1}, {1, 1}, {2, 1}, {3, 1} },
  { {0, 2}, {1, 2}, {2, 2}, {3, 2} },
  { {0, 3}, {1, 3}, {2, 3}, {3, 3} },
  { {0, 4}, {1, 4}, {2, 4}, {3, 4} },
  { {1, 0}, {2, 0}, {3, 0}, {4, 0} },
  { {1, 1}, {2, 1}, {3, 1}, {4, 1} },
  { {1, 2}, {2, 2}, {3, 2}, {4, 2} },
  { {1, 3}, {2, 3}, {3, 3}, {4, 3} },
  { {1, 4}, {2, 4}, {3, 4}, {4, 4} },
  { {0, 1}, {1, 2}, {2, 3}, {3, 4} },
  { {0, 0}, {1, 1}, {2, 2}, {3, 3} },
  { {1, 1}, {2, 2}, {3, 3}, {4, 4} },
  { {1, 0}, {2, 1}, {3, 2}, {4, 3} },
  { {0, 3}, {1, 2}, {2, 1}, {3, 0} },
  { {0, 4}, {1, 3}, {2, 2}, {3, 1} },
  { {1, 3}, {2, 2}, {3, 1}, {4, 0} },
  { {1, 4}, {2, 3}, {3, 2}, {4, 1} }
};

const int lose[48][3][2] = {
  { {0, 0}, {0, 1}, {0, 2} }, { {0, 1}, {0, 2}, {0, 3} }, { {0, 2}, {0, 3}, {0, 4} },
  { {1, 0}, {1, 1}, {1, 2} }, { {1, 1}, {1, 2}, {1, 3} }, { {1, 2}, {1, 3}, {1, 4} },
  { {2, 0}, {2, 1}, {2, 2} }, { {2, 1}, {2, 2}, {2, 3} }, { {2, 2}, {2, 3}, {2, 4} },
  { {3, 0}, {3, 1}, {3, 2} }, { {3, 1}, {3, 2}, {3, 3} }, { {3, 2}, {3, 3}, {3, 4} },
  { {4, 0}, {4, 1}, {4, 2} }, { {4, 1}, {4, 2}, {4, 3} }, { {4, 2}, {4, 3}, {4, 4} },
  { {0, 0}, {1, 0}, {2, 0} }, { {1, 0}, {2, 0}, {3, 0} }, { {2, 0}, {3, 0}, {4, 0} },
  { {0, 1}, {1, 1}, {2, 1} }, { {1, 1}, {2, 1}, {3, 1} }, { {2, 1}, {3, 1}, {4, 1} },
  { {0, 2}, {1, 2}, {2, 2} }, { {1, 2}, {2, 2}, {3, 2} }, { {2, 2}, {3, 2}, {4, 2} },
  { {0, 3}, {1, 3}, {2, 3} }, { {1, 3}, {2, 3}, {3, 3} }, { {2, 3}, {3, 3}, {4, 3} },
  { {0, 4}, {1, 4}, {2, 4} }, { {1, 4}, {2, 4}, {3, 4} }, { {2, 4}, {3, 4}, {4, 4} },
  { {0, 2}, {1, 3}, {2, 4} }, { {0, 1}, {1, 2}, {2, 3} }, { {1, 2}, {2, 3}, {3, 4} },
  { {0, 0}, {1, 1}, {2, 2} }, { {1, 1}, {2, 2}, {3, 3} }, { {2, 2}, {3, 3}, {4, 4} },
  { {1, 0}, {2, 1}, {3, 2} }, { {2, 1}, {3, 2}, {4, 3} }, { {2, 0}, {3, 1}, {4, 2} },
  { {0, 2}, {1, 1}, {2, 0} }, { {0, 3}, {1, 2}, {2, 1} }, { {1, 2}, {2, 1}, {3, 0} },
  { {0, 4}, {1, 3}, {2, 2} }, { {1, 3}, {2, 2}, {3, 1} }, { {2, 2}, {3, 1}, {4, 0} },
  { {1, 4}, {2, 3}, {3, 2} }, { {2, 3}, {3, 2}, {4, 1} }, { {2, 4}, {3, 3}, {4, 2} }
};

class Position {
public:
    int boardState[5][5];

    Position() {
        for (int i = 0; i < 5; i++)
            for (int j = 0; j < 5; j++)
            boardState[i][j] = 0;
    }

    Position(int board[5][5]) {
        for (int i = 0; i < 5; ++i)
            for (int j = 0; j < 5; ++j)
                boardState[i][j] = board[i][j];
    }

    // Generate all legal moves (encoded as two-digit row/col)
    std::vector<int> getLegalMoves(bool maximizingPlayer) const {
        std::vector<int> children;
        for (int i = 0; i < 5; ++i) {
            for (int j = 0; j < 5; ++j) {
                if (boardState[i][j] == 0) {
                    int move = (i + 1) * 10 + (j + 1);
                    children.emplace_back(move);
                }
            }
        }
        return children;
    }

    // Place a move on the board; returns false if invalid
    bool setMove(int move, int player) {
        int i = (move / 10) - 1;
        int j = (move % 10) - 1;
        if (i < 0 || i > 4 || j < 0 || j > 4) return false;
        if (boardState[i][j] != 0) return false;
        boardState[i][j] = player;
        return true;
    }

    // Undo a move; returns false if the cell didn't contain the player's mark
    bool undoMove(int move, int player) {
        int i = (move / 10) - 1;
        int j = (move % 10) - 1;
        if (i < 0 || i > 4 || j < 0 || j > 4) return false;
        if (boardState[i][j] != player) return false;
        boardState[i][j] = 0;
        return true;
    }

    // Heuristic evaluation
    int evaluate(bool maximizingPlayer) const;

    // Detect opponent's imminent win: any 4-cell line with exactly 3 of player and 1 empty
    bool imminentWin(int player) const {
        for (int i = 0; i < 28; ++i) {
            int cnt = 0, empt = 0;
            for (int k = 0; k < 4; ++k) {
                int r = win[i][k][0];
                int c = win[i][k][1];
                int v = boardState[r][c];
                if (v == player) cnt++;
                else if (v == 0) empt++;
            }
            if (cnt == 3 && empt == 1) return true;
        }
        return false;
    }

    bool winningCheck(int player) const {
        for (int k = 0; k < 28; ++k) {
            if (boardState[win[k][0][0]][win[k][0][1]] == player &&
                boardState[win[k][1][0]][win[k][1][1]] == player &&
                boardState[win[k][2][0]][win[k][2][1]] == player &&
                boardState[win[k][3][0]][win[k][3][1]] == player) {
                return true;
            }
        }
        return false;
    }

    bool losingCheck(int player) const {
        for (int k = 0; k < 48; ++k) {
            if (boardState[lose[k][0][0]][lose[k][0][1]] == player &&
                boardState[lose[k][1][0]][lose[k][1][1]] == player &&
                boardState[lose[k][2][0]][lose[k][2][1]] == player) {
                return true;
            }
        }
        return false;
    }

    void printBoard() {
        std::cout << "  1 2 3 4 5\n";
        for (int i = 0; i < 5; i++) {
            std::cout << "%d" << i+1;
            for (int j = 0; j < 5; j++)
            switch (boardState[i][j]) {
                case 0: printf(" -"); break;
                case 1: printf(" X"); break;
                case 2: printf(" O"); break;
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }
};