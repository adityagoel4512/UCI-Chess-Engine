#include "moveGenerator.h"
#include <vector>
#include <iostream>
namespace MoveGeneration {

    MoveGenerator::MoveGenerator(AdiChess::Board const &board_): board(board_) {
        generateLegalMoves();
    }


    // Sliding piece attacks
    template<Piece::Type pieceType>
    void MoveGenerator::generateMoves() {
        Side currentPlayer = board.getCurrentPlayer();
        uint64_t positions = board.getPositions(Piece(pieceType, currentPlayer));
        uint64_t currentSidePieceMap = board.getPositions(board.getCurrentPlayer());

        while(positions) {
            uint64_t position = Utility::bitScanForward(positions);
            uint64_t attackedPositions = getAttackMap(position, 1, pieceType);
            while (attackedPositions) {
                uint64_t attackedPosition = Utility::bitScanForward(attackedPositions);
                moves.emplace_back(position, attackedPosition, Move::Flag::CAPTURE);
                Utility::clearBit(attackedPositions, attackedPosition);
            }
            Utility::clearBit(positions, position);
        }
    }

    // Specialisation for pawns to add consideration for promotions, push/double push moves (not part of the attack map)
    template<>
    void MoveGenerator::generateMoves<Piece::Type::P>() {

    }

    // Specialisation for king to add consideration for castling (castling is classed as a king's move  )
    template<>
    void MoveGenerator::generateMoves<Piece::Type::K>() {

    }

    // TODO:
    // 1) Attacks for sliding pieces as normal with own pieces consideration DONE
    // 2) Pawn push move consideration
    // 3) Castling consideration
    // 4) En Passant Consideration
    // 5) Promotion Consideration

    uint64_t MoveGenerator::getAttackMap(uint64_t position, uint64_t oppositionOccupied, Piece::Type const pieceType) {
        uint64_t friendlyOccupied = board.getPositions(board.getCurrentPlayer());
        switch (pieceType) {
            case Piece::Type::P:
                return MoveGeneration::pawnAttacks[board.getCurrentPlayer()][position] & ~friendlyOccupied;
            case Piece::Type::K:
                return MoveGeneration::pawnAttacks[0][position] | MoveGeneration::pawnAttacks[1][position] | MoveGeneration::getFileAttacks(friendlyOccupied, -1, position) | MoveGeneration::getRankAttacks(friendlyOccupied, -1, position);
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