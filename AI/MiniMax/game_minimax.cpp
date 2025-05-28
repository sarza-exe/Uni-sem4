#include <iostream>
#include <string>
#include <cstring>
#include <chrono>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <gsl/gsl_rng.h>
#include <time.h>
#include <vector>

#include "position.h"

// Heuristic: count partial lines of length 5 without enemy
        static const int lines[12][5][2] = {
            // Rows
            { {0,0},{0,1},{0,2},{0,3},{0,4} },
            { {1,0},{1,1},{1,2},{1,3},{1,4} },
            { {2,0},{2,1},{2,2},{2,3},{2,4} },
            { {3,0},{3,1},{3,2},{3,3},{3,4} },
            { {4,0},{4,1},{4,2},{4,3},{4,4} },
            // Columns
            { {0,0},{1,0},{2,0},{3,0},{4,0} },
            { {0,1},{1,1},{2,1},{3,1},{4,1} },
            { {0,2},{1,2},{2,2},{3,2},{4,2} },
            { {0,3},{1,3},{2,3},{3,3},{4,3} },
            { {0,4},{1,4},{2,4},{3,4},{4,4} },
            // Diagonals
            { {0,0},{1,1},{2,2},{3,3},{4,4} },
            { {0,4},{1,3},{2,2},{3,1},{4,0} }
        };

int Position::evaluate(bool maximizingPlayer) const {
    int me  = maximizingPlayer ? 1 : 2;
    int you = 3 - me;

    // 1) Terminal shortcuts
    if (winningCheck(me))  return +10000;
    if (winningCheck(you)) return -10000;
    if (losingCheck(me))   return -10000;
    if (losingCheck(you))  return +10000;

    int score = 0;

    // 2) Center control bonus
    //    Encourage occupying central squares
    constexpr int centerWeights[5][5] = {
      { 1, 2, 3, 2, 1 },
      { 2, 4, 6, 4, 2 },
      { 3, 6, 9, 6, 3 },
      { 2, 4, 6, 4, 2 },
      { 1, 2, 3, 2, 1 }
    };
    for (int i = 0; i < 5; ++i)
      for (int j = 0; j < 5; ++j)
        if (boardState[i][j] == me)
          score += centerWeights[i][j];
        else if (boardState[i][j] == you)
          score -= centerWeights[i][j];

    // 3) Count all 4-length windows (potential wins) in each line
    //    and all 3-length windows (immediate threats)
    auto countWindows = [&](int length, int side) {
      int w = 0;
      for (auto &line : lines) {
        // slide window of size `length` along the 5-cell line
        for (int start = 0; start + length <= 5; ++start) {
          bool hasEnemy = false;
          int  cntMe    = 0;
          for (int k = 0; k < length; ++k) {
            int v = boardState[line[start + k][0]]
                                [line[start + k][1]];
            if (v == (3 - side)) { hasEnemy = true; break; }
            if (v == side)       ++cntMe;
          }
          if (!hasEnemy) {
            // assign score by how many of your stones in the window
            if      (cntMe == length)        w += 100;  // already win
            else if (cntMe == length - 1)    w += 20;   // immediate
            else if (cntMe == length - 2)    w += 5;    // promising
            else if (cntMe == 1)             w += 1;
            else                              w += 0;
          }
        }
      }
      return w;
    };

    // my potential 4-in-a-row vs opponent’s
    int my4windows  = countWindows(4, me);
    int your4windows= countWindows(4, you);
    int my3windows  = countWindows(3, me);
    int your3windows= countWindows(3, you);

    score +=  50 * my4windows;
    score -= 100 * your4windows;  // block opponent 4’s aggressively
    score +=  10 * my3windows;
    score -=  30 * your3windows;  // penalize their immediate threats

    return score;
}


int minimax(Position &position, int depth, int alpha, int beta, bool maximizingPlayer)
{
    int player = maximizingPlayer ? 1 : 2;
    int enemy = maximizingPlayer ? 2 : 1;

    if (position.imminentWin(player)) return +10000 + depth;  // you have 3 in a 4-cell window with one empty
    if (position.imminentWin(enemy)) return -10000 + depth;  // you have 3 in a 4-cell window with one empty
    if (position.winningCheck(player)) return +10000 + depth; // win now is best
    if (position.winningCheck(enemy)) return -10000 - depth; // you win next
    if (position.losingCheck(player)) return -10000 - depth; // 3-in-a-row = instant loss
    if (position.losingCheck(enemy)) return +10000 + depth; // opponent set-3 = they lose
    
    // depth 0 - return evaluation
    if (depth == 0) {
        int aaa = position.evaluate(maximizingPlayer);
        std::cout << "Evaluation = "<<aaa<<" for board:\n";
        position.printBoard();
        return aaa;
    }

    if (maximizingPlayer) {
        int maxEval = std::numeric_limits<int>::min();
        // For each possible move
        for (const int &move : position.getLegalMoves(true)) {
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

int openingMove(Position &position, int player){
    std::cout << "OPENING\n";
    if(position.boardState[1][1] == 0) return 22;
    if(position.boardState[3][1] == 0) return 42;
    if(position.boardState[1][3] == 0) return 24;
    if(position.boardState[3][3] == 0) return 44;
    
    for(int i = 1; i < 4; i++)
        for(int j = 1; j < 4; j++){
            if(position.boardState[i][j] == 0) {
                int move = (i+1)*10+j+1;
                position.setMove(move, player);
                if(!position.losingCheck(player)) {
                    position.undoMove(move, player);
                    return move;
                }
                position.undoMove(move, player);
            }
        }
    return 0;
}

int main(int argc, char* argv[]) {
    int initBoard[5][5] = {
        { 2, 2, 0, 0, 0 },
        { 1, 2, 1, 2, 0 },
        { 0, 0, 1, 0, 0 },
        { 0, 1, 1, 0, 0 },
        { 2, 0, 1, 0, 0 }
    };

    // 2) Pass it into your Position ctor
    Position test(initBoard);
    std::cout << "Eval for test: " << test.evaluate(false) << "\n";

    if (argc != 6) {
        std::cerr << "Wrong number of arguments\n";
        return EXIT_FAILURE;
    }

    const std::string server_ip   = argv[1];
    const int         server_port = std::stoi(argv[2]);
    const std::string player_id   = argv[3];
    const std::string player_pass = argv[4];
    const int         depth       = std::stoi(argv[5]);

    // Initialize GSL RNG
    gsl_rng* generator = gsl_rng_alloc(gsl_rng_mt19937);
    gsl_rng_set(generator, static_cast<unsigned long>(time(nullptr)));

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
            std::cout << "Move nr. " << no_move << "\n";
            if(no_move < depth && no_move < 5) move = openingMove(current_pos, player);
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
    gsl_rng_free(generator);
    return EXIT_SUCCESS;
}
