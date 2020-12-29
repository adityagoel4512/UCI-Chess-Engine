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
extern uint64_t kingAttacks[64];

void init();

uint64_t getRankAttacks(uint64_t friendlyOccupied, uint64_t oppositionOccupied, uint64_t position);
uint64_t getFileAttacks(uint64_t friendlyOccupied, uint64_t oppositionOccupied, uint64_t position);
uint64_t getDiagonalAttacks(uint64_t friendlyOccupied, uint64_t oppositionOccupied, uint64_t position);
uint64_t getAntiDiagonalAttacks(uint64_t friendlyOccupied, uint64_t oppositionOccupied, uint64_t position);

std::string positionToString(uint64_t position);

enum: uint64_t {
    rank1 = 0x00000000000000FF,
    rank2 = 0x000000000000FF00,
    rank3 = 0x0000000000FF0000,
    rank4 = 0x00000000FF000000,
    rank5 = 0x000000FF00000000,
    rank6 = 0x0000FF0000000000,
    rank7 = 0x00FF000000000000,
    rank8 = 0xFF00000000000000,
};

enum: uint64_t {
    fileH = 0x101010101010101,
    fileG = 0x202020202020202,
    fileF = 0x404040404040404,
    fileE = 0x808080808080808,
    fileD = 0x1010101010101010,
    fileC = 0x2020202020202020,
    fileB = 0x4040404040404040,
    fileA = 0x8080808080808080,
};

}