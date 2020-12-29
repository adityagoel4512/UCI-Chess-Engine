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
            bool killKing = (1ULL << move.getTo()) == board.getPositions(Piece(Piece::Type::K, board.getOpponent()));
            if (killKing) {
                    os << "kill\n";
                    os << move.getFlag() << '\n';
                    os << board << '\n';
            }

            board.makeMove(move);

            if (killKing) { 
                os << board << '\n';
            }

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
    fs.open("results.txt");
    cout << "perft 5: " << perft(5, board, fs) << '\n';
    fs.close();
    cout << "captures: " << captures << '\n';
    cout << "promotions: " << promotions << '\n';
    cout << "castles: " << castles << '\n';
    cout << "en passant: " << enPassant << '\n';
    cout << "check mates " << checkmates << '\n';
    cout << "checks " << checks << '\n';

    return EXIT_SUCCESS;
}