#include <iostream>
#include <string>
#include "board.h"
int main(int argc, char *argv[]) {
    std::cout << "Let's play chess!\n";

    AdiChess::Board board;
    std::cout << board << '\n';
    return EXIT_SUCCESS;
}