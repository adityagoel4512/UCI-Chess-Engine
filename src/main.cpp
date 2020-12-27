#include <iostream>
#include <string>
#include "board.h"
#include "moveGenerator.h"

int main() {
    std::cout << "Let's play chess!\n";

    AdiChess::Board board;
    std::cout << board << '\n';
    auto moves = MoveGeneration::MoveGenerator(board);
    auto move = *(moves.begin()+1);
    board.makeMove(move);
    std::cout << board << '\n';
    board.unmakeMove(move);
    std::cout << board << '\n';


    
    // for (auto move : moves) {
    //     std::cout << "From " << move.getFrom() << " to " << move.getTo() << "\n";
    //     std::cout << board.legalMove(move) << "\n";
    // }
    // AdiChess::Move move(3, 3+40, AdiChess::Move::Flag::QUIET_MOVE);
    // std::cout << board.legalMove(move) << "\n";
    // std::cout << board.getCurrentPlayer() << "\n";
    // std::cout << moves.size() << '\n';

    // AdiChess::Board board2("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQK2R w KQkq - 0 1");
    // std::cout << board2 << '\n';
    // AdiChess::Move move2(0, 0, AdiChess::Move::Flag::KING_CASTLE);
    // std::cout << board2.legalMove(move2) << '\n';
    // board2.makeMove(move2);
    // std::cout << board2 << '\n';
    return EXIT_SUCCESS;
}