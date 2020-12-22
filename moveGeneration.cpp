#include "moveGeneration.h"

namespace MoveGeneration {

uint64_t rayAttacks[Direction::NUM_DIRECTIONS][64] = {0};
uint64_t pawnAttacks[2][64] = {0};
uint64_t knightAttacks[64] = {0};

uint64_t eastN(uint64_t board, int n);
uint64_t westN(uint64_t board, int n);

uint64_t getRayAttacks(uint64_t occupied, Direction direction, uint64_t position) {
    uint64_t attacks = rayAttacks[direction][position];
    uint64_t blocker = attacks & occupied;
    // If no blockers we can take attacks as is
    if (blocker) {
        // Checks if positive ray attack or negative ray attack
        if (direction <= Direction::E) {
            Utility::bitScanForward(blocker);
        } else {
            Utility::bitScanReverse(blocker);
        }
        attacks ^= rayAttacks[direction][blocker];
    }

    return attacks;
}


uint64_t getRankAttacks(uint64_t occupied, uint64_t position) {
    return getRayAttacks(occupied, Direction::N, position) | getRayAttacks(occupied, Direction::S, position);
}

uint64_t getFileAttacks(uint64_t occupied, uint64_t position) {
    return getRayAttacks(occupied, Direction::E, position) | getRayAttacks(occupied, Direction::W, position);
}

uint64_t getDiagonalAttacks(uint64_t occupied, uint64_t position) {
    return getRayAttacks(occupied, Direction::NE, position) | getRayAttacks(occupied, Direction::NW, position);
}

uint64_t getAntiDiagonalAttacks(uint64_t occupied, uint64_t position) {
    return getRayAttacks(occupied, Direction::SE, position) | getRayAttacks(occupied, Direction::SW, position);
}

// Initialises rays, pawn and knight attack maps. Invoked once at engine start up.
void init() {

    for (uint64_t position = 0; position < 64; ++position) {
        rayAttacks[Direction::N][position] = 0x0101010101010100ULL << position;
        rayAttacks[Direction::S][position] = 0x0080808080808080ULL >> (63 - position);
        uint8_t bitsRight = position % 8ULL;
        rayAttacks[Direction::E][position] = ((1ULL << bitsRight) - 1ULL) << (position - bitsRight);
        rayAttacks[Direction::W][position] = position == 64 ? 0 : ((1ULL << 7 - bitsRight) - 1ULL) << (position + 1);
        rayAttacks[Direction::NE][position] = eastN(0x102040810204000ULL, 7ULL - Utility::getCol(position)) << (Utility::getRow(position) << 3);
        rayAttacks[Direction::NW][position] = westN(0x8040201008040200ULL, Utility::getCol(position)) << (Utility::getRow(position) << 3);
        rayAttacks[Direction::SE][position] = eastN(0x40201008040201ULL, 7ULL - Utility::getCol(position)) >> ((7ULL - Utility::getRow(position)) << 3);
        rayAttacks[Direction::SW][position] = westN(0x2040810204080ULL, Utility::getCol(position)) >> ((7ULL - Utility::getRow(position)) << 3);
    }

    for (uint64_t position = 0; position < 64; ++position) {
        pawnAttacks[0][position] = getRayAttacks(-1, Direction::NW, position) | getRayAttacks(-1, Direction::NE, position);
        pawnAttacks[1][position] = getRayAttacks(-1, Direction::SW, position) | getRayAttacks(-1, Direction::SE, position);

        uint64_t pos = (1ULL << position);
        int column = Utility::getCol(position);
        int row = Utility::getRow(position);
        if (column != 7) {
            knightAttacks[position] |= (pos << 17);
            knightAttacks[position] |= (pos >> 15);
            if (column != 6) {
                knightAttacks[position] |= (pos << 10);
                knightAttacks[position] |= (pos >> 6);
            }
        }

        if (column != 0) {
            knightAttacks[position] |= (pos << 15);
            knightAttacks[position] |= (pos >> 17);
            if (column != 1) {
                knightAttacks[position] |= (pos << 6);
                knightAttacks[position] |= (pos >> 10);
            }
        }
        
    }
}

uint64_t eastN(uint64_t board, int n) {
    uint64_t newBoard = board;
    for (int i = 0; i < n; i++) {
        newBoard = ((newBoard >> 1) & (~(0x8080808080808080ULL)));
     }

    return newBoard;
}

uint64_t westN(uint64_t board, int n) {
  uint64_t newBoard = board;
  for (int i = 0; i < n; i++) {
    newBoard = ((newBoard << 1) & (~0x101010101010101ULL));
  }

  return newBoard;
}


}