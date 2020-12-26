#include <iostream>
#include <string>
#include "board.h"
#include "moveGenerator.h"

int main() {
    std::cout << "Let's play chess!\n";

    AdiChess::Board board;
    std::cout << board << '\n';
    auto moves = MoveGeneration::MoveGenerator(board);

    for (auto move : moves) {
        std::cout << "From " << move.getFrom() << " to " << move.getTo() << "\n";
    }
    std::cout << moves.size() << '\n';
    return EXIT_SUCCESS;
}