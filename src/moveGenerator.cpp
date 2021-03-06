#include "moveGenerator.h"
#include <iostream>
namespace MoveGeneration {

MoveGenerator::MoveGenerator(AdiChess::Board const &board_)
    : board{board_}, currentPlayer{board_.getCurrentPlayer()},
      friendlyOccupied{board.getPositions(currentPlayer)},
      oppositionOccupied{board.getPositions(board_.getOpponent())} {
    generatePseudoLegalMoves();
}

// Sliding piece pseudo legal moves
template <Piece::Type pieceType> void MoveGenerator::generateMoves() {

    uint64_t positions = board.getPositions(pieceType, currentPlayer);

    while (positions) {
        uint64_t position = Utility::bitScanForward(positions);
        generateAttackMoves<pieceType>(position);
        Utility::clearBit(positions, position);
    }
}

template <Piece::Type pieceType>
void MoveGenerator::generateAttackMoves(uint64_t position) {
    assert(position <= 63);
    uint64_t attackedPositions =
        board.getAttackMap(position, pieceType, friendlyOccupied,
                           oppositionOccupied, currentPlayer);
    while (attackedPositions) {
        uint64_t attackedPosition = Utility::bitScanPop(attackedPositions);
        assert(attackedPosition != position);
        if (oppositionOccupied & (1ULL << attackedPosition)) {
            moves.emplace_back(position, attackedPosition, Move::CAPTURE);
        } else {
            moves.emplace_back(position, attackedPosition, Move::QUIET_MOVE);
        }
    }
}

// Specialisation for King to add consideration for castling
template <> void MoveGenerator::generateMoves<Piece::Type::K>() {
    uint64_t position = board.getPositions(Piece::Type::K, currentPlayer);

    if (position == 0)
        std::cerr << "No king error\n" << board << '\n';

    assert(position != 0);

    position = Utility::bitScanForward(position);
    // Generates all non-castling moves
    generateAttackMoves<Piece::Type::K>(position);

    // Generates pseudo legal castling moves
    if (board.canKingSideCastle(oppositionOccupied | friendlyOccupied)) {
        moves.emplace_back(0, 0, Move::KING_CASTLE);
    }

    if (board.canQueenSideCastle(oppositionOccupied | friendlyOccupied)) {
        moves.emplace_back(0, 0, Move::QUEEN_CASTLE);
    }
}

template <>
void MoveGenerator::generatePawnPushMove<Side::B>(uint64_t pawnPosition,
                                                  uint64_t freePositions) {
    // Single pawn push
    uint64_t singlePawnPushPosition = pawnPosition - 8;
    uint64_t singlePawnPush = (1ULL << singlePawnPushPosition) & freePositions;
    if (singlePawnPush & rank1) {
        moves.emplace_back(pawnPosition, singlePawnPushPosition,
                           Move::KNIGHT_PROMOTION);
        moves.emplace_back(pawnPosition, singlePawnPushPosition,
                           Move::BISHOP_PROMOTION);
        moves.emplace_back(pawnPosition, singlePawnPushPosition,
                           Move::ROOK_PROMOTION);
        moves.emplace_back(pawnPosition, singlePawnPushPosition,
                           Move::QUEEN_PROMOTION);
    } else if (singlePawnPush) {
        uint64_t singlePawnPushPosition = pawnPosition - 8;
        moves.emplace_back(pawnPosition, singlePawnPushPosition,
                           Move::QUIET_MOVE);
        // Double pawn push
        uint64_t doublePawnPush = (singlePawnPush >> 8) & rank5 & freePositions;
        if (doublePawnPush) {
            moves.emplace_back(pawnPosition, singlePawnPushPosition - 8,
                               Move::DOUBLE_PAWN_PUSH);
        }
    }
}

template <>
void MoveGenerator::generatePawnPushMove<Side::W>(uint64_t pawnPosition,
                                                  uint64_t freePositions) {
    // Single pawn push
    uint64_t singlePawnPushPosition = pawnPosition + 8;
    uint64_t singlePawnPush = (1ULL << singlePawnPushPosition) & freePositions;
    if (singlePawnPush & rank8) {
        moves.emplace_back(pawnPosition, singlePawnPushPosition,
                           Move::KNIGHT_PROMOTION);
        moves.emplace_back(pawnPosition, singlePawnPushPosition,
                           Move::BISHOP_PROMOTION);
        moves.emplace_back(pawnPosition, singlePawnPushPosition,
                           Move::ROOK_PROMOTION);
        moves.emplace_back(pawnPosition, singlePawnPushPosition,
                           Move::QUEEN_PROMOTION);
    } else if (singlePawnPush) {
        moves.emplace_back(pawnPosition, singlePawnPushPosition,
                           Move::QUIET_MOVE);
        // Double pawn push
        uint64_t doublePawnPush = (singlePawnPush << 8) & rank4 & freePositions;
        if (doublePawnPush) {
            moves.emplace_back(pawnPosition, singlePawnPushPosition + 8,
                               Move::DOUBLE_PAWN_PUSH);
        }
    }
}

// Specialisation for pawns to add consideration for promotions, push moves and
// en passant captures
template <> void MoveGenerator::generateMoves<Piece::Type::P>() {
    uint64_t positions = board.getPositions(Piece::Type::P, currentPlayer);
    uint64_t oppositionOccupied = board.getPositions(board.getOpponent());
    uint64_t freePositions = ~(friendlyOccupied | oppositionOccupied);

    while (positions) {
        uint64_t position = Utility::bitScanPop(positions);
        // Generates diagonal (non en-passant attacks from position)
        uint64_t attackedPositions =
            board.getAttackMap(position, Piece::Type::P, friendlyOccupied,
                               oppositionOccupied, currentPlayer);
        while (attackedPositions) {
            uint64_t attackedPosition = Utility::bitScanPop(attackedPositions);
            if ((1ULL << attackedPosition) & promotionRank[currentPlayer]) {
                // Promotion capture
                moves.emplace_back(position, attackedPosition,
                                   Move::KNIGHT_PROMO_CAPTURE);
                moves.emplace_back(position, attackedPosition,
                                   Move::BISHOP_PROMO_CAPTURE);
                moves.emplace_back(position, attackedPosition,
                                   Move::ROOK_PROMO_CAPTURE);
                moves.emplace_back(position, attackedPosition,
                                   Move::QUEEN_PROMO_CAPTURE);
            } else {
                // Standard capture
                moves.emplace_back(position, attackedPosition, Move::CAPTURE);
            }
        }

        // Generates push moves
        if (currentPlayer == Side::W) {
            generatePawnPushMove<Side::W>(position, freePositions);
        } else if (currentPlayer == Side::B) {
            generatePawnPushMove<Side::B>(position, freePositions);
        }

        // Generates en passant moves
        uint64_t pawnAttacks =
            MoveGeneration::pawnAttacks[currentPlayer][position];
        while (pawnAttacks) {
            uint64_t pawnAttack = Utility::bitScanPop(pawnAttacks);
            if (board.validEnPassant(pawnAttack)) {
                moves.emplace_back(position, pawnAttack,
                                   Move::EN_PASSANT_CAPTURE);
                break;
            }
        }
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

} // namespace MoveGeneration