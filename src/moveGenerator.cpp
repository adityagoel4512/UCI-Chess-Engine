#include "moveGenerator.h"
#include <vector>
#include <iostream>
namespace MoveGeneration {

    MoveGenerator::MoveGenerator(AdiChess::Board const &board_): 
        board(board_), 
        currentPlayer(board_.getCurrentPlayer()), 
        friendlyOccupied(board.getPositions(currentPlayer)),
        oppositionOccupied(board.getPositions(board_.getOpponent())) {
        generatePseudoLegalMoves();
    }


    // Sliding piece pseudo legal moves
    template<Piece::Type pieceType>
    void MoveGenerator::generateMoves() {
        
        uint64_t positions = board.getPositions(Piece(pieceType, currentPlayer));

        while(positions) {
            uint64_t position = Utility::bitScanForward(positions);
            assert(position <= 63);
            generateAttackMoves<pieceType>(position);
            Utility::clearBit(positions, position);
        }
    }

    template<Piece::Type pieceType>
    void MoveGenerator::generateAttackMoves(uint64_t position) {
        assert(position <= 63);
        uint64_t attackedPositions = board.getAttackMap(position, pieceType, friendlyOccupied, oppositionOccupied, currentPlayer);

        while (attackedPositions) {
            uint64_t attackedPosition = Utility::bitScanForward(attackedPositions);
            if (oppositionOccupied & (1ULL << attackedPosition)) {
                moves.emplace_back(position, attackedPosition, Move::Flag::CAPTURE);
            } else {
                moves.emplace_back(position, attackedPosition, Move::Flag::QUIET_MOVE);
            }
            Utility::clearBit(attackedPositions, attackedPosition);
        }
    }

    // Specialisation for King to add consideration for castling
    template<>
    void MoveGenerator::generateMoves<Piece::Type::K>() {
        uint64_t position = board.getPositions(Piece(Piece::Type::K, currentPlayer));

        if (position == 0) {
            std::cout << board << '\n';
        }

        assert(position != 0);

        position = Utility::bitScanForward(position);
        // Generates all non-castling moves
        assert(position <= 63);
        generateAttackMoves<Piece::Type::K>(position);

        // Generates pseudo legal castling moves
        if (board.canKingSideCastle(oppositionOccupied | friendlyOccupied)) {
            moves.emplace_back(0, 0, Move::Flag::KING_CASTLE);
        }

        if (board.canQueenSideCastle(oppositionOccupied | friendlyOccupied)) {
            moves.emplace_back(0, 0, Move::Flag::QUEEN_CASTLE);
        }
    }
    
    template<>
    void MoveGenerator::generatePawnPushMove<Side::B>(uint64_t pawnPosition, uint64_t freePositions) {
        // Single pawn push
        uint64_t pawnBitMap = 1ULL << pawnPosition;
        uint64_t singlePawnPush = (pawnBitMap >> 8) & freePositions;
        if (singlePawnPush & rank1) {
            uint64_t singlePawnPushPosition = pawnPosition-8;
            moves.emplace_back(pawnPosition, singlePawnPushPosition, Move::Flag::KNIGHT_PROMOTION);
            moves.emplace_back(pawnPosition, singlePawnPushPosition, Move::Flag::BISHOP_PROMOTION);
            moves.emplace_back(pawnPosition, singlePawnPushPosition, Move::Flag::ROOK_PROMOTION);
            moves.emplace_back(pawnPosition, singlePawnPushPosition, Move::Flag::QUEEN_PROMOTION);
        } else if (singlePawnPush) {
            uint64_t singlePawnPushPosition = pawnPosition-8;
            moves.emplace_back(pawnPosition, singlePawnPushPosition, Move::Flag::QUIET_MOVE);
            // Double pawn push
            uint64_t doublePawnPush = (singlePawnPush >> 8) & rank5 & freePositions;
            if (doublePawnPush) {
                moves.emplace_back(pawnPosition, singlePawnPushPosition-8, Move::Flag::DOUBLE_PAWN_PUSH);
            }
        }
    }

    template<>
    void MoveGenerator::generatePawnPushMove<Side::W>(uint64_t pawnPosition, uint64_t freePositions) {
        // Single pawn push
        uint64_t pawnBitMap = 1ULL << pawnPosition;
        uint64_t singlePawnPush = (pawnBitMap << 8) & freePositions;
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
            uint64_t attackedPositions = board.getAttackMap(position, Piece::Type::P, friendlyOccupied, oppositionOccupied, currentPlayer);
            while (attackedPositions) {
                uint64_t attackedPosition = Utility::bitScanForward(attackedPositions);
                if ((1ULL << attackedPosition) & promotionRank[currentPlayer]) {
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
    
    void MoveGenerator::generatePseudoLegalMoves() {
        generateMoves<Piece::Type::K>();
        generateMoves<Piece::Type::Q>();
        generateMoves<Piece::Type::R>();
        generateMoves<Piece::Type::B>();
        generateMoves<Piece::Type::N>();
        generateMoves<Piece::Type::P>();
    }
    
}