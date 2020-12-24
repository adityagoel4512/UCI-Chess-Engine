#include <iostream>
#include <string>
#include "board.h"
int main(int argc, char *argv[]) {
    std::cout << "Let's play chess!\n";

    AdiChess::Board board;
    std::cout << board << '\n';
    AdiChess::Move move(Utility::flattenCoordinates(6, 2), Utility::flattenCoordinates(4, 2), 0);
    board.makeMove(move);
    std::cout << board << '\n';
    return EXIT_SUCCESS;
}