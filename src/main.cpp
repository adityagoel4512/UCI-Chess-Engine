#include <iostream>
#include <fstream>
#include <string>

#include "search.h"
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
    Board board;
    Search search(board);
    int score = search.negamax(4);
    std::cout << board << '\n';
    std::cout << score << '\n';
    std::cout << search.getPrincipalMove().getFrom() << '\n';
    board.makeMove(search.getPrincipalMove());
    std::cout << board << '\n';
    board.makeMove({MoveGeneration::e7, MoveGeneration::e5, Move::DOUBLE_PAWN_PUSH});
    std::cout << board << '\n';
    std::cout << search.negamax(4) << '\n';
    board.makeMove(search.getPrincipalMove());
    std::cout << board << '\n';
    board.makeMove({MoveGeneration::b7, MoveGeneration::b5, Move::DOUBLE_PAWN_PUSH});
    std::cout << board << '\n';
    std::cout << search.negamax(4) << '\n';
    board.makeMove(search.getPrincipalMove());
    std::cout << board << '\n';
    board.makeMove({MoveGeneration::b8, MoveGeneration::c6, Move::QUIET_MOVE});
    std::cout << board << '\n';
    std::cout << search.negamax(4) << '\n';
    board.makeMove(search.getPrincipalMove());
    std::cout << board << '\n';
    // board.makeMove({MoveGeneration::e5, MoveGeneration::f4, Move::CAPTURE});
    // std::cout << board << '\n';
    // std::cout << search.negamax(4) << '\n';
    // board.makeMove(search.getPrincipalMove());
    // std::cout << board << '\n';
    return EXIT_SUCCESS;
}