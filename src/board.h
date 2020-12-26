#pragma once

#include "piece.h"

namespace AdiChess {

class Board {

public:
    explicit Board(std::string const &fenString = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    Piece operator()(int position) const;
    void operator()(int position, Piece const &piece);

    void makeMove(Move const &move);
    bool legalMove(Move const &move);

    uint64_t getPositions(Piece const &piece) const;
    uint64_t getPositions(Side const &side) const;

    bool canQueenSideCastle(uint64_t oppostionAttacks) const;
    bool canKingSideCastle(uint64_t oppostionAttacks) const;

    uint64_t getAttackMap(uint64_t position, Piece::Type const pieceType, uint64_t friendlyOccupied, uint64_t oppositionOccupied, Side const &side) const;

    bool legalMove(Move const &move, uint64_t oppositionAttacks, uint64_t oppositionPositions, uint64_t friendlyPositions);

    Side getCurrentPlayer() const {
        return currentPlayer;
    }

    Side getOpponent() const {
        return opponent;
    }

    bool validEnPassant(uint64_t position) const {
        return position == enPassantTarget;
    }

    friend std::ostream &operator<<(std::ostream &os, Board const &board) {
        os << std::string("Full moves: ") << std::to_string(board.fullMoveNumber) << std::string("\n");
        os << std::string("Side to move: ");

        if (board.currentPlayer == Side::W) {
            os << std::string("White");
        } else if (board.currentPlayer == Side::B) {
            os << std::string("Black");
        } else if (board.currentPlayer == Side::NONE) {
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
    void setPiece(uint64_t position);
    void clearPiece(int position, Piece const &piece);
    void movePiece(uint64_t from, uint64_t to);

    void makeCapture(Move const &move);
    void makePromotion(Move const &move);
    void makeQueenSideCastle();
    void makeKingSideCastle();

    template<Piece::Type pieceType> 
    bool legalNonKingMove(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t) const;
    bool legalEnPassantMove(Move const &move, uint64_t oppositionPositions, uint64_t friendlyPositions, uint64_t kingPosition);

    Piece operator() (int i, int j) const;
    void operator()(int i, int j, Piece const &piece);

    void parseFenString(std::string const &fenString);

    uint64_t bitboards[6][2] = {0};
    uint8_t castlingRights = 0;
    int halfMoveClock = 0;
    int fullMoveNumber = 0;
    uint64_t enPassantTarget = -1;
    Side currentPlayer;
    Side opponent;
};

}