#pragma once
#include <stdio.h>
#include "position.h"

int board[5][5];

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
