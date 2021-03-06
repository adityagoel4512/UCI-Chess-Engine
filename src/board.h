#pragma once

#include "piece.h"

#include <stack>
#include <memory>

#define DEBUG 1

namespace AdiChess {

struct StateInfo {

    StateInfo(int halfMoveClock_, int fullMoveNumber_, uint64_t enPassantTarget_, uint8_t castlingRights_, std::shared_ptr<StateInfo> prev_, std::string descr_= ""): 
    halfMoveClock(halfMoveClock_), fullMoveNumber(fullMoveNumber_), enPassantTarget(enPassantTarget_), castlingRights(castlingRights_), prev(prev_), descr(descr_) {}

    int halfMoveClock = 0;
    int fullMoveNumber = 0;
    uint64_t enPassantTarget;
    uint8_t castlingRights = 0;
    std::shared_ptr<StateInfo> prev;
    Piece capturedPiece{Piece::Type::NONE, Side::NONE};
    std::string descr;
};

class Board {

public:
    explicit Board(std::string const &fenString = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    Board(const Board &) = delete;
    Board& operator=(const Board &) = delete;
    
    Piece operator()(int position) const;
    void operator()(int position, Piece const &piece);

    Board& makeMove(Move const &move);
    void unmakeMove(Move const &move);
    bool legalMove(Move const &move);
    bool inCheck(Side const &side) const;
    bool fiftyMoves() const;

    uint64_t getPositions(Piece::Type const &pieceType, Side const &side) const;
    uint64_t getPositions(Side const &side) const;

    bool canQueenSideCastle(uint64_t occupiedPositions) const;
    bool canKingSideCastle(uint64_t occupiedPositions) const;

    uint64_t getAttackMap(uint64_t position, Piece::Type const &pieceType, uint64_t friendlyOccupied, uint64_t oppositionOccupied, Side const &side) const;

    bool legalMove(Move const &move, uint64_t oppositionAttacks, uint64_t oppositionPositions, uint64_t friendlyPositions);

    Side getCurrentPlayer() const {
        return currentPlayer;
    }

    Side getOpponent() const {
        return opponent;
    }

    bool validEnPassant(uint64_t position) const {
        return position == state->enPassantTarget;
    }

    friend std::ostream &operator<<(std::ostream &os, Board const &board) {
#if DEBUG
        os << std::string("Description: ") << board.state->descr << std::string("\n");
#endif
        os << std::string("Castling: ") << std::to_string(board.state->castlingRights) << std::string("\n");
        os << std::string("Full moves: ") << std::to_string(board.state->fullMoveNumber) << std::string("\n");
        os << std::string("En passant: ") << std::to_string(board.state->enPassantTarget) << std::string("\n");
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
            os << std::string("| ") << std::to_string(8-i) << std::string("\n+---+---+---+---+---+---+---+---+\n");
        }
        
        return os << std::string("  a   b   c   d   e   f   g   h\n");
    }

private:
    void clearPiece(int position, Piece const &piece);
    void movePiece(uint64_t from, uint64_t to);

    void makeCapture(Move const &move);
    void makePromotion(Move const &move);
    void makeQueenSideCastle();
    void makeKingSideCastle();
    void unmakeQueenSideCastle();
    void unmakeKingSideCastle();

    uint64_t getAbsolutePinRay(uint64_t attackSources, uint64_t defenderPosition);

    bool legalEnPassantMove(Move const &move);

    void updateState();
    void updateCastlingBits(Piece const &piece, uint64_t moveSource);

    Piece operator() (int i, int j) const;
    void operator()(int i, int j, Piece const &piece);

    void parseFenString(std::string const &fenString);

    uint64_t bitboards[6][2] = {0};
    uint64_t aggregateBitboards[2] = {0};

    Side currentPlayer;
    Side opponent;

    // StateInfo is allocated on free store for now.
    std::shared_ptr<StateInfo> state;
};

}