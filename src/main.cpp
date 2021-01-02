#include <iostream>
#include <fstream>
#include <string>

#include "board.h"
#include "moveGenerator.h"
uint64_t captures = 0; 
uint64_t promotions = 0; 
uint64_t castles = 0; 
uint64_t enPassant = 0; 
uint64_t checkmates = 0; 
uint64_t checks = 0; 


using namespace std;
using namespace Utility;


uint64_t dividePerftRes[64][64] = {0};

uint64_t dividePerft(int depth, Board &board, int cur = 0) {
    if (cur == depth)
        return 1;

    MoveGeneration::MoveGenerator moveGen(board);
    uint64_t ans = 0;
    for (auto const &move : moveGen) {
        if (board.legalMove(move)) {
            board.makeMove(move);
            uint64_t val = dividePerft(depth, board, cur+1);
            // if (cur == 0) {
                dividePerftRes[move.getFrom()][move.getTo()] += val;
            // }
            board.unmakeMove(move);
            ans += val;
        }
    }

    return ans;
}

int main() {
    return EXIT_SUCCESS;
}