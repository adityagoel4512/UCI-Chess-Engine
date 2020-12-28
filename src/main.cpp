#include <iostream>
#include <fstream>

#include <string>
#include "board.h"
#include "moveGenerator.h"
uint64_t captures = 0; 
uint64_t promotions = 0; 
uint64_t castles = 0; 
uint64_t perft(int depth, AdiChess::Board &board, std::ostream &os) {
    if (depth == 0) {
        // os << board << '\n';
        return 1;
    }
    uint64_t nodes = 0;
    MoveGeneration::MoveGenerator moveGenerator(board);
    for (auto move : moveGenerator) {
        if (board.legalMove(move)) {
            if (move.isCapture()) {
                captures++;
            }

            if (move.isPromotion()) {
                promotions++;
            }

            if (move.getFlag() == AdiChess::Move::Flag::KING_CASTLE || move.getFlag() == AdiChess::Move::Flag::QUEEN_CASTLE) {
                castles++;
            }
            board.makeMove(move);
            nodes += perft(depth-1, board, os);
            board.unmakeMove(move);
        }
    }
    return nodes;
}
using namespace std;
using namespace Utility;

int main() {
    
    AdiChess::Board board;
    MoveGeneration::MoveGenerator moveGen(board);
    std::ofstream fs;
    fs.open("r.txt");
    cout << "perft 3: " << perft(3, board, fs) << '\n';
    fs.close();
    cout << "captures: " << captures << '\n';
    cout << "promotions: " << promotions << '\n';
    cout << "castles: " << castles << '\n';

    return EXIT_SUCCESS;
}