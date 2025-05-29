#include <iostream>
#include <string>
#include <cstring>
#include <chrono>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <vector>

#include "position.h"
#include "opening.h"

/**
 * @brief Heuristic evaluation for a 5×5 Tic-Tac-Toe.
 * 
 * This function computes a numeric score for the current position from the perspective of the
 * “me” player (1). Positive scores favor “me”, negative scores favor the opponent “you” (2).
 * 
 * Scoring steps:
 * 1. Terminal checks:
 *    - If the opponent has a winning 4-in-a-row: return –10000 (loss).
 *    - If “me” has a winning 4-in-a-row: return +10000 (win).
 *    - If “me” has an unintended 3-in-a-row (losing condition): return –10000.
 *    - If the opponent has a 3-in-a-row (they lose): return +10000.
 * 
 * 2. Center‐control bonus:
 *    - Adds weights based on a predefined 5×5 matrix, rewarding central positions more.
 *    - Each “me” mark adds its cell weight; each “you” mark subtracts it.
 * 
 * 3. Four‐cell window analysis:
 *    - For each of the 28 possible 4-cell lines, count marks:
 *      • If only “me” occupies the window, add:
 *          – +5 for 1 mark, +20 for 2, +50 for 3 (gap threat), +200 for 4 (already won).
 *      • If only “you” occupies it, subtract:
 *          – –5 for 1 mark, –20 for 2, –400 for 3 (imminent threat), –200 for 4 (they won).
 * 
 * @return Integer score, with large magnitude for wins/losses and moderate values
 *         guiding the search in non-terminal positions.
 */
int Position::evaluate() const {
    int me  = 1;
    int you = 2;
    int score = 0;

    // 1) Terminal
    if (winningCheck(you)) return -10000;
    if (winningCheck(me)) return 10000;
    if (losingCheck(me))   return -10000;
    if (losingCheck(you))  return 10000;
    
    // 2) Center bonus
    constexpr int centerW[5][5] = {
      {1, 2, 3, 2, 1},
      {2, 4, 6, 4, 2},
      {3, 6, 4, 6, 3},
      {2, 4, 6, 4, 2},
      {1, 2, 3, 2, 1}
    };
    
    for (int i = 0; i < 5; ++i)
      for (int j = 0; j < 5; ++j) {
        if (boardState[i][j] == me)  score += centerW[i][j];
        else if (boardState[i][j] == you) score -= centerW[i][j];
      }

    // 3) Lines of 4 (from position.h win[28][4][2])
    for (int w = 0; w < 28; ++w) {
      int meC = 0, youC = 0;
      for (int k = 0; k < 4; ++k) {
        int r = fourInARow[w][k][0], c = fourInARow[w][k][1];
        if      (boardState[r][c] == me)  ++meC;
        else if (boardState[r][c] == you) ++youC;
      }

      // only one side occupies this window
      if (youC == 0 && meC > 0) {
        switch (meC) {
          case 1: score += 5; break;   // tiny advance
          case 2: score += 20; break;   // decent
          case 3: score += 50; break;   // huge “gap” threat
          case 4: score += 200; break;   // already win (though terminal caught it)
        }
      }
      else if (meC == 0 && youC > 0) {
        switch (youC) {
          case 1: score -= 5; break;
          case 2: score -= 20; break;
          case 3: score -= 400; break;   // punish imminent opponent four
          case 4: score -= 200; break;   // they already win
        }
      }
    }
    return score;
}


int minimax(Position &position, int depth, int alpha, int beta, bool maximizingPlayer)
{
    int player = maximizingPlayer ? 1 : 2;
    int enemy = 3 - player;

    // No need to check any further
    if (position.winningCheck(1)) return +10000 + depth;
    if (position.winningCheck(2)) return -10000 - depth;
    if (position.losingCheck(2)) return +10000 + depth;
    if (position.losingCheck(1)) return -10000 - depth;

    // return evaluation
    if (depth == 0) {
        return position.evaluate();
    }

    if (maximizingPlayer) {
        int maxEval = std::numeric_limits<int>::min();
        // For each possible move
        auto moves = position.getLegalMoves(true);
        for (const int &move : moves) {
            position.setMove(move, player);
            int eval = minimax(position, depth - 1, alpha, beta, false);
            position.undoMove(move, player);
            maxEval = std::max(maxEval, eval);
            alpha   = std::max(alpha, eval);
            if (beta <= alpha) {
                break;  // beta-cutoff
            }
        }
        return maxEval;
    } else {
        int minEval = std::numeric_limits<int>::max();
        for (const int &move : position.getLegalMoves(false)) {
            position.setMove(move, player);
            int eval = minimax(position, depth - 1, alpha, beta, true);
            position.undoMove(move, player);
            minEval = std::min(minEval, eval);
            beta    = std::min(beta, eval);
            if (beta <= alpha) {
                break;  // alpha-cutoff
            }
        }
        return minEval;
    }
}

int findBestMove(Position &root, int depth, bool maximizingPlayer) {
    int player = maximizingPlayer ? 1 : 2;

    int bestValue = maximizingPlayer ? std::numeric_limits<int>::min() : std::numeric_limits<int>::max();
    int alpha = std::numeric_limits<int>::min();
    int beta  = std::numeric_limits<int>::max();
    int bestMove = 0;

    for (const int &move : root.getLegalMoves(maximizingPlayer)) {
        root.setMove(move, player);
        int val = minimax(root, depth - 1, alpha, beta, !maximizingPlayer);
        std::cout << "Evaluation = "<<val<<" for board:\n";
        root.printBoard();
        root.undoMove(move, player);
        if ((maximizingPlayer && val > bestValue) || (!maximizingPlayer && val < bestValue)){
            bestValue = val;
            bestMove = move;
        }
        if (maximizingPlayer) alpha = std::max(alpha, bestValue);
        else beta  = std::min(beta,  bestValue);

        if (beta <= alpha) break;
    }
    return bestMove;
}

// Connecting to server and managing connection by Prof. Maciej Gębala (CC BY-NC 4.0)
// Modified for educational use
int main(int argc, char* argv[]) {
    int initBoard[5][5] = {
        { 1, 2, 1, 1, 0 },
        { 0, 1, 2, 0, 0 },
        { 0, 0, 2, 0, 0 },
        { 0, 2, 0, 1, 0 },
        { 0, 0, 0, 2, 0 }
    };
    Position test(initBoard);
    std::cout << "Eval for test: " << test.evaluate() << "\n";
    if (argc != 6) {
        std::cerr << "Wrong number of arguments\n";
        return EXIT_FAILURE;
    }

    const std::string server_ip   = argv[1];
    const int         server_port = std::stoi(argv[2]);
    const std::string player_id   = argv[3];
    const std::string player_pass = argv[4];
    const int         depth       = std::stoi(argv[5]);

    // Create socket
    int sock = ::socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Unable to create socket\n";
        return EXIT_FAILURE;
    }
    std::cout << "Socket created successfully\n";

    // Server address setup
    sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(server_port);
    server_addr.sin_addr.s_addr = inet_addr(server_ip.c_str());

    // Connect
    if (connect(sock, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) < 0) {
        std::cerr << "Unable to connect\n";
        close(sock);
        return EXIT_FAILURE;
    }
    std::cout << "Connected with server successfully\n";

    // Receive initial server message (unused)
    char server_message[16] = {0};
    if (recv(sock, server_message, sizeof(server_message), 0) < 0) {
        std::cerr << "Error while receiving server's message\n";
        close(sock);
        return EXIT_FAILURE;
    }

    // Send credentials: "<id> <pass>"
    std::string creds = player_id + " " + player_pass;
    if (send(sock, creds.c_str(), creds.size(), 0) < 0) {
        std::cerr << "Unable to send message\n";
        close(sock);
        return EXIT_FAILURE;
    }

    Position board;
    bool   end_game = false;
    int    player;
    std::sscanf(player_id.c_str(), "%d", &player);
    std::cout << "depth is " << depth << "\n";

    bool is_maximizing = player == 2 ? false : true;
    std::cout << "is_maximizing = " << is_maximizing << " for player " << player << "\n";

    int no_move = 0;

    while (!end_game) {
        std::memset(server_message, 0, sizeof(server_message));
        if (recv(sock, server_message, sizeof(server_message), 0) < 0) {
            std::cerr << "Error while receiving server's message\n";
            break;
        }

        int msg_total;
        std::sscanf(server_message, "%d", &msg_total);
        int move = msg_total % 100;
        int msg  = msg_total / 100;

        if (move != 0) {
            board.setMove(move, 3 - player);
        }

        if (msg == 0 || msg == 6) {
            // our turn
            Position current_pos(board.boardState);
            no_move += 1;
            std::cout << "Move nr. " << no_move << "!\n";
            if(no_move < depth && no_move <= no_opening_moves) move = openingMove(current_pos, player);
            else{ 
                move = findBestMove(current_pos, depth, is_maximizing); 
            }
            board.setMove(move, player);

            std::string reply = std::to_string(move);
            if (send(sock, reply.c_str(), reply.size(), 0) < 0) {
                std::cerr << "Unable to send message\n";
                break;
            }
        } else {
            end_game = true;
            switch (msg) {
                case 1: std::cout << "You won.\n"; break;
                case 2: std::cout << "You lost.\n"; break;
                case 3: std::cout << "Draw.\n"; break;
                case 4: std::cout << "You won. Opponent error.\n"; break;
                case 5: std::cout << "You lost. Your error.\n"; break;
                default: std::cout << "Unknown endgame code: " << msg << "\n"; break;
            }
        }
    }

    close(sock);
    return EXIT_SUCCESS;
}
