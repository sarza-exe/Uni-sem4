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
#include <random> //????

#include "board.h"

int minimax(int player, int depth) {
    return 33;
}

class Position {
private:
    int boardState[5][5];
public:
    Position(int board[5][5]) {
        for (int i = 0; i < 5; ++i)
            for (int j = 0; j < 5; ++j)
                boardState[i][j] = board[i][j];
    }
    int evaluate() const;
    std::vector<Position> generateChildren(bool maximizingPlayer) const;
    // … your own board/state data and constructors …
};

int Position::evaluate() const {
    if (winCheck(1)) return +1000;   // AI wins
    if (winCheck(2)) return -1000;   // Opponent wins
    if (loseCheck(1)) return -500;   // AI loses early
    if (loseCheck(2)) return +500;   // Opponent loses early
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> bin(0, 40);
    return bin(rng);  // Neutral
}

std::vector<Position> Position::generateChildren(bool maximizingPlayer) const {
    std::vector<Position> children;

    int player = maximizingPlayer ? 1 : 2;

    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            if (boardState[i][j] == 0) {
                int newBoard[5][5];
                
                // Copy current board state
                for (int x = 0; x < 5; ++x)
                    for (int y = 0; y < 5; ++y)
                        newBoard[x][y] = boardState[x][y];
                
                // Make the move
                newBoard[i][j] = player;

                // Create a new position and add it to children
                children.emplace_back(newBoard);
            }
        }
    }

    return children;
}


int minimax(const Position &position, int depth, int alpha, int beta, bool maximizingPlayer)
{
    // Terminal or depth-0: return evaluation
    if (depth == 0) {
        return position.evaluate();
    }

    if (maximizingPlayer) {
        int maxEval = std::numeric_limits<int>::min();
        // For each possible move
        for (const Position &child : position.generateChildren(true)) {
            int eval = minimax(child, depth - 1, alpha, beta, false);
            maxEval = std::max(maxEval, eval);
            alpha   = std::max(alpha, eval);
            if (beta <= alpha) {
                break;  // beta-cutoff
            }
        }
        return maxEval;
    } else {
        int minEval = std::numeric_limits<int>::max();
        for (const Position &child : position.generateChildren(false)) {
            int eval = minimax(child, depth - 1, alpha, beta, true);
            minEval = std::min(minEval, eval);
            beta    = std::min(beta, eval);
            if (beta <= alpha) {
                break;  // alpha-cutoff
            }
        }
        return minEval;
    }
}

int main(int argc, char* argv[]) {
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

    setBoard();
    bool   end_game = false;
    int    player;
    std::sscanf(player_id.c_str(), "%d", &player);
    std::cout << "depth is " << depth << "\n";

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
            setMove(move, 3 - player);
        }

        if (msg == 0 || msg == 6) {
            // our turn
            move = minimax(player, depth);
            setMove(move, player);

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
