#include "piece.h"
namespace AdiChess {

    uint64_t Piece::getAttackMap(uint64_t position, uint64_t occupied) {
        switch (type) {
            case Type::P:
                return MoveGeneration::pawnAttacks[side][position];
            case Type::K:
                return MoveGeneration::pawnAttacks[0][position] | MoveGeneration::pawnAttacks[1][position] | MoveGeneration::getFileAttacks(-1, position) | MoveGeneration::getRankAttacks(-1, position);
            case Type::N:   
                return MoveGeneration::knightAttacks[position];
            case Type::B:
                return MoveGeneration::getDiagonalAttacks(occupied, position) | MoveGeneration::getAntiDiagonalAttacks(occupied, position);
            case Type::R:
                return MoveGeneration::getFileAttacks(occupied, position) | MoveGeneration::getRankAttacks(occupied, position);
            case Type::Q:
                return MoveGeneration::getDiagonalAttacks(occupied, position) | MoveGeneration::getAntiDiagonalAttacks(occupied, position) 
                | MoveGeneration::getFileAttacks(occupied, position) | MoveGeneration::getRankAttacks(occupied, position);
            default:
                break;
        }
        return 0;
    }

}