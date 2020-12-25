#include "moveGenerator.h"
#include <vector>
#include <iostream>
namespace MoveGeneration {

    MoveGenerator::MoveGenerator(AdiChess::Board const &board_): 
        board(board_), 
        currentPlayer(board_.getCurrentPlayer()), 
        friendlyOccupied(board.getPositions(currentPlayer)),
        oppositionOccupied(board.getPositions(board_.getOpponent())) {
        generateLegalMoves();
    }


    // Sliding piece pseudo legal moves
    template<Piece::Type pieceType>
    void MoveGenerator::generateMoves() {
        uint64_t positions = board.getPositions(Piece(pieceType, currentPlayer));
        while(positions) {
            uint64_t position = Utility::bitScanForward(positions);
            generateAttackMoves<pieceType>(position);
            Utility::clearBit(positions, position);
        }
    }

    template<Piece::Type pieceType>
    void MoveGenerator::generateAttackMoves(uint64_t position) {
        uint64_t attackedPositions = getAttackMap(position, pieceType);
        while (attackedPositions) {
            uint64_t attackedPosition = Utility::bitScanForward(attackedPositions);
            moves.emplace_back(position, attackedPosition, Move::Flag::CAPTURE);
            Utility::clearBit(attackedPositions, attackedPosition);
        }
    }
    
    // Specialisation for King to add consideration for castling
    template<>
    void MoveGenerator::generateMoves<Piece::Type::K>() {
        uint64_t position = Utility::bitScanForward(board.getPositions(Piece(Piece::Type::K, currentPlayer)));

        // Generates all non-castling moves
        generateAttackMoves<Piece::Type::K>(position);

        // Generates castling moves
        uint64_t oppositionMap = oppositionOccupied;
        uint64_t oppositionAttacks = 0;

        while (oppositionMap) {
            uint64_t oppositionPosition = Utility::bitScanForward(oppositionMap);
            oppositionAttacks |= oppositionPosition;
            oppositionAttacks |= getAttackMap(position, board(oppositionPosition).type);
            Utility::clearBit(oppositionMap, oppositionPosition);
        }        

        if (board.canKingSideCastle(oppositionAttacks)) {
            moves.emplace_back(0, 0, Move::Flag::KING_CASTLE);
        }

        if (board.canQueenSideCastle(oppositionAttacks)) {
            moves.emplace_back(0, 0, Move::Flag::QUEEN_CASTLE);
        }
    }
    
    template<>
    void MoveGenerator::generatePawnPushMove<Side::B>(uint64_t pawnPosition, uint64_t freePositions) {
        // Single pawn push
        uint64_t pawnBitMap = 1ULL << pawnPosition;
        uint64_t singlePawnPush = pawnBitMap >> 8;
        if (singlePawnPush & rank8) {
            uint64_t singlePawnPushPosition = pawnPosition-8;
            moves.emplace_back(pawnPosition, singlePawnPushPosition, Move::Flag::KNIGHT_PROMOTION);
            moves.emplace_back(pawnPosition, singlePawnPushPosition, Move::Flag::BISHOP_PROMOTION);
            moves.emplace_back(pawnPosition, singlePawnPushPosition, Move::Flag::ROOK_PROMOTION);
            moves.emplace_back(pawnPosition, singlePawnPushPosition, Move::Flag::QUEEN_PROMOTION);
        } else if (singlePawnPush) {
            uint64_t singlePawnPushPosition = pawnPosition-8;
            moves.emplace_back(pawnPosition, singlePawnPushPosition, Move::Flag::QUIET_MOVE);
            // Double pawn push
            uint64_t doublePawnPush = (singlePawnPush << 8) & rank4 & freePositions;
            if (doublePawnPush) {
                moves.emplace_back(pawnPosition, singlePawnPushPosition-8, Move::Flag::DOUBLE_PAWN_PUSH);
            }
        }
    }

    template<>
    void MoveGenerator::generatePawnPushMove<Side::W>(uint64_t pawnPosition, uint64_t freePositions) {
        // Single pawn push
        uint64_t pawnBitMap = 1ULL << pawnPosition;
        uint64_t singlePawnPush = pawnBitMap << 8;
        if (singlePawnPush & rank8) {
            uint64_t singlePawnPushPosition = pawnPosition+8;
            moves.emplace_back(pawnPosition, singlePawnPushPosition, Move::Flag::KNIGHT_PROMOTION);
            moves.emplace_back(pawnPosition, singlePawnPushPosition, Move::Flag::BISHOP_PROMOTION);
            moves.emplace_back(pawnPosition, singlePawnPushPosition, Move::Flag::ROOK_PROMOTION);
            moves.emplace_back(pawnPosition, singlePawnPushPosition, Move::Flag::QUEEN_PROMOTION);
        } else if (singlePawnPush) {
            uint64_t singlePawnPushPosition = pawnPosition+8;
            moves.emplace_back(pawnPosition, singlePawnPushPosition, Move::Flag::QUIET_MOVE);
            // Double pawn push
            uint64_t doublePawnPush = (singlePawnPush << 8) & rank4 & freePositions;
            if (doublePawnPush) {
                moves.emplace_back(pawnPosition, singlePawnPushPosition+8, Move::Flag::DOUBLE_PAWN_PUSH);
            }
        }
    }

    // Specialisation for pawns to add consideration for promotions, push moves and en passant captures
    template<>
    void MoveGenerator::generateMoves<Piece::Type::P>() {
        uint64_t positions = board.getPositions(Piece(Piece::Type::P, currentPlayer));
        uint64_t oppositionOccupied = board.getPositions(board.getOpponent());
        uint64_t freePositions = ~(friendlyOccupied | oppositionOccupied);

        while(positions) {
            uint64_t position = Utility::bitScanForward(positions);
            // Generates diagonal (non en-passant attacks from position)
            uint64_t attackedPositions = getAttackMap(position, Piece::Type::P);
            while (attackedPositions) {
                uint64_t attackedPosition = Utility::bitScanForward(attackedPositions);
                if (attackedPosition & promotionRank[currentPlayer]) {
                    // Promotion capture
                    moves.emplace_back(position, attackedPosition, Move::Flag::KNIGHT_PROMO_CAPTURE);
                    moves.emplace_back(position, attackedPosition, Move::Flag::BISHOP_PROMO_CAPTURE);
                    moves.emplace_back(position, attackedPosition, Move::Flag::ROOK_PROMO_CAPTURE);
                    moves.emplace_back(position, attackedPosition, Move::Flag::QUEEN_PROMO_CAPTURE);
                } else {
                    // Standard capture
                    moves.emplace_back(position, attackedPosition, Move::Flag::CAPTURE);
                }
                Utility::clearBit(attackedPositions, attackedPosition);
            }

            // Generates push moves
            if (currentPlayer == Side::W) {
                generatePawnPushMove<Side::W>(position, freePositions);
            } else if (currentPlayer == Side::B) {
                generatePawnPushMove<Side::B>(position, freePositions);
            }

            // Generates en passant moves
            uint64_t pawnAttacks = MoveGeneration::pawnAttacks[currentPlayer][position];
            while (pawnAttacks) {
                uint64_t pawnAttack = Utility::bitScanForward(pawnAttacks);
                if (board.validEnPassant(pawnAttack)) {
                    moves.emplace_back(position, pawnAttack, Move::Flag::EN_PASSANT_CAPTURE);
                    break;
                }
                Utility::clearBit(pawnAttacks, pawnAttack);
            }
            
            Utility::clearBit(positions, position);
        }
    }

    uint64_t MoveGenerator::getAttackMap(uint64_t position, Piece::Type const pieceType) {
        switch (pieceType) {
            case Piece::Type::P:
                return MoveGeneration::pawnAttacks[currentPlayer][position] & oppositionOccupied;
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
    
    void MoveGenerator::generateLegalMoves() {
        generateMoves<Piece::Type::K>();
        generateMoves<Piece::Type::Q>();
        generateMoves<Piece::Type::R>();
        generateMoves<Piece::Type::B>();
        generateMoves<Piece::Type::N>();
        generateMoves<Piece::Type::P>();
    }
    
}