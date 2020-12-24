#pragma once
#include "move.h"

#include <cstdint>
#include <cassert>

// Classical approach

namespace MoveGeneration {

enum Direction {
    NW, N, NE, E, SE, S, SW, W, NUM_DIRECTIONS=8
};

extern uint64_t rayAttacks[Direction::NUM_DIRECTIONS][64];
extern uint64_t pawnAttacks[2][64];
extern uint64_t knightAttacks[64];

void init();

uint64_t getRankAttacks(uint64_t occupied, uint64_t position);
uint64_t getFileAttacks(uint64_t occupied, uint64_t position);
uint64_t getDiagonalAttacks(uint64_t occupied, uint64_t position);
uint64_t getAntiDiagonalAttacks(uint64_t occupied, uint64_t position);

uint64_t getRayAttacks(uint64_t occupied, Direction direction, uint64_t position);

}