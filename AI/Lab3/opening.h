#pragma once
#include <stdio.h>
#include "position.h"

// Up to this many “book” moves before opening
int no_opening_moves = 2;
// Remember which preferred squares we’ve already taken
static std::vector<int> opening_history;
// The four “good” candidates
static const int preferred[4] = { 22, 42, 24, 44 };

// Returns true if a+b is one of the two forbidden diagonals
static bool isDiagonalPair(int a, int b) {
    // 22-44 or 42-24 are the only forbidden combos
    return ( (a==22 && b==44) || (a==44 && b==22)
          || (a==42 && b==24) || (a==24 && b==42) );
}

int openingMove(Position &position, int player){
    const int enemy = 3 - player;
    std::cout << "OPENING\n";

    // 1) Block any immediate 4-in-a-row threat:
    if (position.imminentWin(enemy)) {
        for (int mv : position.getLegalMoves(player)) {
            Position tmp = position;
            tmp.setMove(mv, player);
            if (!tmp.imminentWin(enemy)) {
                return mv;
            }
        }
    }
    std::cout<<"Check 1. ";

    for (int i = 0; i < 4; ++i) {
        int mv = preferred[i];
        int r = (mv/10)-1, c = (mv%10)-1;
        if (position.boardState[r][c] != 0) continue;      // already occupied

        bool bad = false;
        for (int prev : opening_history) {
            if (isDiagonalPair(prev, mv)) {
                bad = true;
                break;
            }
        }
        if (bad) continue;

        // accept this move
        opening_history.push_back(mv);
        return mv;
    }

    std::cout<<"Check 2. ";

    // 3) Fallback: first safe central non‐losing
    for (int i = 1; i < 4; ++i) {
        for (int j = 1; j < 4; ++j) {
            if (position.boardState[i][j] == 0) {
                int mv = (i+1)*10 + (j+1);
                for (int prev : opening_history) {
                    if (isDiagonalPair(prev, mv)) {
                        continue;;
                    }
                }
                Position tmp = position;
                tmp.setMove(mv, player);
                if (!tmp.losingCheck(player)) return mv;
            }
        }
    }

    std::cout<<"Check 3. ";

    // 4) Last resort: any legal move
    auto moves = position.getLegalMoves(player);
    if (!moves.empty()) {
        return moves.front();
    }
    return 0;
}
