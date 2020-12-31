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

uint64_t perft(int depth, AdiChess::Board &board, std::ostream &os) {
    if (depth == 0) {
        return 1;
    }
    uint64_t nodes = 0;
    MoveGeneration::MoveGenerator moveGenerator(board);
    
    for (auto move : moveGenerator) {

        if (board.legalMove(move)) {
            // os << "=================================================\n";
            // os << move.getFlag() << '\n';
            // os << MoveGeneration::positionToString(move.getFrom()) << MoveGeneration::positionToString(move.getTo()) << '\n';
            // os << board << '\n';
            board.makeMove(move);
            // os << board << '\n';

            if (depth == 1) {
                if (board.inCheck(board.getCurrentPlayer())) {
                    checks++;
                }

                if (move.isCapture()) {
                    captures++;
                }

                if (move.isPromotion()) {
                    promotions++;
                }

                if (move.getFlag() == AdiChess::Move::Flag::EN_PASSANT_CAPTURE) {
                    enPassant++;
                }

                if (move.getFlag() == AdiChess::Move::Flag::KING_CASTLE || move.getFlag() == AdiChess::Move::Flag::QUEEN_CASTLE) {
                    castles++;
                }

            }
            
            nodes += perft(depth-1, board, os);
            board.unmakeMove(move);
            // os << board << '\n';

        }
    }

    return nodes;
}
using namespace std;
using namespace Utility;


uint64_t dividePerftRes[64][64] = {0};

uint64_t dividePerft(int depth, Board &board, std::ostream &os, int cur = 0) {
    if (cur == depth)
        return 1;

    MoveGeneration::MoveGenerator moveGen(board);
    uint64_t ans = 0;
    for (auto const &move : moveGen) {
        if (board.legalMove(move)) {
            board.makeMove(move);
            uint64_t val = dividePerft(depth, board, os, cur+1);
            if (cur == 0) {
                dividePerftRes[move.getFrom()][move.getTo()] += val;
            }
            board.unmakeMove(move);
            ans += val;
        }
    }

    return ans;
}

int main() {

    // AdiChess::Board board("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
    AdiChess::Board board;
    std::ofstream fs;
    fs.open("results.txt");
    // board.makeMove(Move(1, 0, 0));
    // board.makeMove(Move(63, 55, 0));

    std::cout << board << '\n';
    cout << "perft 2: " << perft(6, board, fs) << '\n';

    for (int i = 0; i <= 63; ++i) {
        for (int j = 0; j <= 63; ++j) {
            if (dividePerftRes[i][j]) {
                fs << MoveGeneration::positionToString(i) << MoveGeneration::positionToString(j) << ": " << dividePerftRes[i][j] << '\n';
            }
        }
    }

    fs.close();
    cout << "captures: " << captures << '\n';
    cout << "promotions: " << promotions << '\n';
    cout << "castles: " << castles << '\n';
    cout << "en passant: " << enPassant << '\n';
    cout << "check mates " << checkmates << '\n';
    cout << "checks " << checks << '\n';

    return EXIT_SUCCESS;
}