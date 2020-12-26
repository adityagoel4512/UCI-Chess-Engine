#include "board.h"
#include <sstream>
#include <iostream>
#include <algorithm>

namespace AdiChess {

    Board::Board(std::string const &fenString) {
        parseFenString(fenString);
        MoveGeneration::init();
    }

    void Board::makeMove(Move const &move) {
        uint64_t source = move.getFrom();
        uint64_t target = move.getTo();
        auto flag = move.getFlag();

        // Update en passant target square if double pawn push
        if (flag == Move::DOUBLE_PAWN_PUSH) {
            if (currentPlayer == Side::W) {
                enPassantTarget = target >> 8;
            } else {
                enPassantTarget = target << 8;
            }
        }
        
        if (move.isCapture()) {
            makeCapture(move);
        } else if (flag == Move::KING_CASTLE) {
            makeKingSideCastle();
        } else if (flag == Move::QUEEN_CASTLE) {
            makeQueenSideCastle();
        } else {
            movePiece(source, target);
        }

        makePromotion(move);

        ++halfMoveClock;
        if (currentPlayer == Side::B) {
            ++fullMoveNumber;
        }

        std::swap(currentPlayer, opponent);
    }

    bool Board::legalMove(Move const &move) const {
        uint64_t friendlyPositions = getPositions(currentPlayer);
        uint64_t oppositionAttacks = 0;
        uint64_t oppositionBitMap = getPositions(opponent);

        while (oppositionBitMap) {
            uint64_t oppositionPosition = Utility::bitScanForward(oppositionBitMap);
            uint64_t attack = getAttackMap(oppositionPosition, (*this)(oppositionPosition).type, getPositions(opponent), 0xFFFFFFFFFFFFFFFF, opponent);
            oppositionAttacks |= attack;
            Utility::clearBit(oppositionBitMap, oppositionPosition);
        }
        
        return legalMove(move, oppositionAttacks, getPositions(opponent), friendlyPositions);
    }

    bool Board::legalMove(Move const &move, uint64_t oppositionAttacks, uint64_t oppositionPositions, uint64_t friendlyPositions) const {
        uint64_t kingPosition = getPositions(Piece(Piece::Type::K, currentPlayer));
        uint64_t fromPosition = 1ULL << move.getFrom();
        uint64_t toPosition = 1ULL << move.getTo();

        // A king move is legal if and only if it does not move into check.
        if (kingPosition == fromPosition)
            return !(oppositionAttacks & toPosition);

        // A non-king move is legal if and only if it is not pinned or it is moving along the ray towards or away from the king along 
        // which it is being attacked. Note that a piece can only be pinned by a sliding piece.
        if (!legalNonKingMove<Piece::Type::B>(oppositionPositions, friendlyPositions, fromPosition, toPosition, kingPosition))
            return false;

        if (!legalNonKingMove<Piece::Type::Q>(oppositionPositions, friendlyPositions, fromPosition, toPosition, kingPosition))
            return false;

        if (!legalNonKingMove<Piece::Type::R>(oppositionPositions, friendlyPositions, fromPosition, toPosition, kingPosition))
            return false;

        return true;        

    }
    
    template <Piece::Type pieceType>
    bool Board::legalNonKingMove(uint64_t oppositionPositions, uint64_t friendlyPositions, uint64_t fromPosition, uint64_t toPosition, uint64_t kingPosition) const {
        uint64_t enemySliderPositions = bitboards[static_cast<int>(pieceType)][opponent];
        while (enemySliderPositions) {
            uint64_t sliderPosition = Utility::bitScanForward(enemySliderPositions);
            uint64_t attacks = getAttackMap(sliderPosition, pieceType, oppositionPositions, friendlyPositions & ~fromPosition, opponent);

            // Check if moved piece was pinned.
            if (attacks & kingPosition) {
                // To be legal move piece must be moved along ray of attack.
                if (getAttackMap(sliderPosition, pieceType, oppositionPositions, ((friendlyPositions & ~fromPosition) | toPosition), opponent) & kingPosition) {
                    return false;
                }
            }

            Utility::clearBit(enemySliderPositions, sliderPosition);
        }

        return true;
    }

    uint64_t Board::getPositions(Piece const &piece) const {
        return bitboards[static_cast<int>(piece.type)][piece.side];
    }

    uint64_t Board::getPositions(Side const &side) const {
        uint64_t allPieces = 0;
        for (int i = 0; i < static_cast<int>(Piece::Type::NUM_PIECES); ++i) {
            allPieces |= bitboards[i][side];
        }
        return allPieces;
    }

    void Board::makeQueenSideCastle() {
        uint64_t kingLocation = currentPlayer == Side::B ? 0x0800000000000000 : 0x0000000000000080;
        movePiece(kingLocation, kingLocation << 2);
        movePiece(kingLocation << 4, kingLocation << 1);
    }

    void Board::makeKingSideCastle() {
        uint64_t kingLocation = currentPlayer == Side::B ? 0x0800000000000000 : 0x0000000000000080;
        movePiece(kingLocation, kingLocation >> 2);
        movePiece(kingLocation >> 3, kingLocation >> 1);
    }

    bool Board::canKingSideCastle(uint64_t oppostionAttacks) const {
        // Check if castling flags permit castling
        bool castleLegal = (castlingRights >> (2 * currentPlayer)) & 0b10;
        if (!castleLegal)
            return false;
        
        // Check if path for castling is clear and opposition does not attack castling path
        uint64_t clearPiecesMask = currentPlayer == Side::W ? 0x0000000000000006 : 0x0600000000000000;
        return !(oppostionAttacks & clearPiecesMask);
    }

    bool Board::canQueenSideCastle(uint64_t oppostionAttacks) const {
        // Check if castling flags permit castling
        bool castleLegal = (castlingRights >> (2 * currentPlayer)) & 0b1;
        if (!castleLegal)
            return false;

        // Check if path for castling is clear and opposition does not attack castling path
        uint64_t clearPiecesMask = currentPlayer == Side::W ? 0x0000000000000070 : 0x7000000000000000;
        return !(oppostionAttacks & clearPiecesMask);
    }

    uint64_t Board::getAttackMap(uint64_t position, Piece::Type const pieceType, uint64_t friendlyOccupied, uint64_t oppositionOccupied, Side const &side) const {
        switch (pieceType) {
            case Piece::Type::P:
                return MoveGeneration::pawnAttacks[side][position] & oppositionOccupied;
            case Piece::Type::K:
                return ((MoveGeneration::pawnAttacks[0][position] | MoveGeneration::pawnAttacks[1][position]) & ~friendlyOccupied) | MoveGeneration::getFileAttacks(friendlyOccupied, -1, position) | MoveGeneration::getRankAttacks(friendlyOccupied, -1, position);
            case Piece::Type::N:   
                return MoveGeneration::knightAttacks[position] & ~friendlyOccupied;
            case Piece::Type::B:
                return MoveGeneration::getDiagonalAttacks(friendlyOccupied, oppositionOccupied, position) | MoveGeneration::getAntiDiagonalAttacks(friendlyOccupied, oppositionOccupied, position);
            case Piece::Type::R:
                return MoveGeneration::getFileAttacks(friendlyOccupied, oppositionOccupied, position) | MoveGeneration::getRankAttacks(friendlyOccupied, oppositionOccupied, position);
            case Piece::Type::Q:
                return MoveGeneration::getDiagonalAttacks(friendlyOccupied, oppositionOccupied, position) | MoveGeneration::getAntiDiagonalAttacks(friendlyOccupied, oppositionOccupied, position) 
                | MoveGeneration::getFileAttacks(friendlyOccupied, oppositionOccupied, position) | MoveGeneration::getRankAttacks(friendlyOccupied, oppositionOccupied, position);
            default:
                break;
        }
        return 0;
    }

    Piece Board::operator()(int position) const {
        for (int i = 0; i < static_cast<int>(Piece::Type::NUM_PIECES); ++i) {
            if (Utility::checkBit(bitboards[i][0], position))
                return {static_cast<Piece::Type>(i), Side::W};
            else if (Utility::checkBit(bitboards[i][1], position))
                return {static_cast<Piece::Type>(i), Side::B};
        }
        return {Piece::Type::NONE, Side::NONE};
    }


    void Board::movePiece(uint64_t from, uint64_t to) {
        Piece piece = (*this)(from);

        // Update castling bits
        if (piece.type == Piece::Type::K) {
            castlingRights &= ~(0b11 << (2 * currentPlayer));
        } else if (piece.type == Piece::Type::R) {
            if (from & MoveGeneration::fileA) {
                castlingRights &= ~(0b1 << (2 * currentPlayer));
            } else if (from & MoveGeneration::fileH) {
                castlingRights &= ~(0b10 << (2 * currentPlayer));
            }
        }

        clearPiece(from, piece);
        (*this)(to, piece);
    }

    void Board::makeCapture(Move const &move) {
        auto moveFlag = move.getFlag();

        if (moveFlag == Move::Flag::EN_PASSANT_CAPTURE) {
            clearPiece(enPassantTarget, (*this)(enPassantTarget));
        } else {
            clearPiece(move.getTo(), (*this)(move.getTo()));
        }

        movePiece(move.getFrom(), move.getTo());
    }

    void Board::makePromotion(Move const &move) {
        auto moveFlag = move.getFlag();
        switch (moveFlag) {
            case Move::Flag::BISHOP_PROMOTION:
            case Move::Flag::BISHOP_PROMO_CAPTURE:
                (*this)(move.getTo(), Piece(Piece::Type::B, currentPlayer));
                break;
            case Move::Flag::KNIGHT_PROMOTION:
            case Move::Flag::KNIGHT_PROMO_CAPTURE:
                (*this)(move.getTo(), Piece(Piece::Type::N, currentPlayer));
                break;
            case Move::Flag::QUEEN_PROMOTION:
            case Move::Flag::QUEEN_PROMO_CAPTURE:
                (*this)(move.getTo(), Piece(Piece::Type::Q, currentPlayer));
                break;
            case Move::Flag::ROOK_PROMOTION:
            case Move::Flag::ROOK_PROMO_CAPTURE:
                (*this)(move.getTo(), Piece(Piece::Type::R, currentPlayer));
                break;
        }
    }

    
    void Board::operator()(int position, Piece const &piece) {
        Utility::setBit(bitboards[static_cast<int>(piece.type)][piece.side], position);
    }
    
    Piece Board::operator()(int i, int j) const {
        return (*this)(Utility::flattenCoordinates(i, j));
    } 

    void Board::operator()(int i, int j, Piece const &piece) {
        (*this)(Utility::flattenCoordinates(i, j), piece);
    }

    void Board::clearPiece(int position, Piece const &piece) {
        Utility::clearBit(bitboards[static_cast<int>(piece.type)][piece.side], position);
    }

    void Board::parseFenString(std::string const &fenString) {        
        std::string token;
        std::stringstream ss(fenString);

        // Parse board into bitboard        
        for (int i = 0; i < 8; ++i) {
            if (i == 8) {
                ss >> token;
            } else {
                std::getline(ss, token, '/');
            }
            int j = 0;
            while (j < 8) {
                if (token[j] <= '8' && token[j] >= '1') {
                    j+=token[j]-'0';
                } else {
                    int position = Utility::flattenCoordinates(i, j);
                    switch (token[j]) {
                        case 'K':
                            Utility::setBit(bitboards[0][0], position);
                            break;
                        case 'k':
                            Utility::setBit(bitboards[0][1], position);
                            break;
                        case 'Q':
                            Utility::setBit(bitboards[1][0], position);
                            break;
                        case 'q':
                            Utility::setBit(bitboards[1][1], position);
                            break;
                        case 'B':
                            Utility::setBit(bitboards[2][0], position);
                            break;
                        case 'b':
                            Utility::setBit(bitboards[2][1], position);
                            break;
                        case 'R':
                            Utility::setBit(bitboards[3][0], position);
                            break;
                        case 'r':
                            Utility::setBit(bitboards[3][1], position);
                            break;
                        case 'N':
                            Utility::setBit(bitboards[4][0], position);
                            break;
                        case 'n':
                            Utility::setBit(bitboards[4][1], position);
                            break;
                        case 'P':
                            Utility::setBit(bitboards[5][0], position);
                            break;
                        case 'p':
                            Utility::setBit(bitboards[5][1], position);
                            break;
                    }
                    ++j;
                }
            }
        }

        ss = std::stringstream(token);
        ss >> token >> token;

        // Side to play
        currentPlayer = token == "w" ? Side::W : Side::B;
        opponent = token == "w" ? Side::B : Side::W;


        ss >> token;

        // Castling rights [Black King Side, Black Queen Side, White King Side, White Queen Side]
        if (token != "-") {
            for (char c : token) {
                if (c == 'K') 
                    castlingRights |= 0b0010;
                else if (c == 'k')
                    castlingRights |= 0b0001;
                else if (c == 'Q') 
                    castlingRights |= 0b1000;
                else if (c == 'q')
                    castlingRights |= 0b0100;
            }
        }

        ss >> token;
        // En passant target square
        if (token != "-") {
            enPassantTarget = Utility::flattenCoordinates(token[0]-'a', token[1]-'0');
        }        

        ss >> token;
        halfMoveClock = std::stoi(token);
        ss >> token;
        fullMoveNumber = std::stoi(token);
    }   
}