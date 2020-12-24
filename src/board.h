#pragma once

#include "piece.h"

namespace AdiChess {

class Board {

public:
    Board(std::string const &fenString = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    Piece operator() (int i, int j) const;
    void operator()(int i, int j, Piece const &piece);
    void makeMove(Move &move);
    // Pretty print
    friend std::ostream &operator<<(std::ostream &os, Board const &board) {
        os << std::string("Full moves: ") << std::to_string(board.fullMoveNumber) << std::string("\n");
        os << std::string("Side to move: ");

        if (board.player == Side::W) {
            os << std::string("White");
        } else if (board.player == Side::B) {
            os << std::string("Black");
        } else if (board.player == Side::NONE) {
            os << std::string("None");
        }

        os << std::string("\n+---+---+---+---+---+---+---+---+\n");
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                os << std::string("| ") << board(i, j) << std::string(" ");
            }
            os << std::string("|\n+---+---+---+---+---+---+---+---+\n");
        }
        
        return os << std::string("  a   b   c   d   e   f   g   h\n");
    }


private:
    uint64_t bitboards[6][2] = {0};
    bool castlingRights[2][2] = {0};
    int halfMoveClock = 0;
    int fullMoveNumber = 0;
    uint64_t enPassantTarget = -1;
    Side player;
    Side opponent;
    void parseFenString(std::string const &fenString);
    void setPiece(uint64_t position);
    Piece operator()(int position) const;
    void operator()(int position, Piece const &piece);
    void clearPiece(int position, Piece const &piece);
    void movePiece(uint64_t from, uint64_t to);
};

}