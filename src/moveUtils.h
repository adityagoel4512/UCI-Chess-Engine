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
    fileA = 0x8080808080808080,
    fileB = 0x4040404040404040,
    fileC = 0x2020202020202020,
    fileD = 0x1010101010101010,
    fileE = 0x0808080808080808,
    fileF = 0x0404040404040404,
    fileG = 0x0202020202020202,
    fileH = 0x0101010101010101,
};

enum: int {
    h1, g1, f1, e1, d1, c1, b1, a1,
    h2, g2, f2, e2, d2, c2, b2, a2,
    h3, g3, f3, e3, d3, c3, b3, a3,
    h4, g4, f4, e4, d4, c4, b4, a4,
    h5, g5, f5, e5, d5, c5, b5, a5,
    h6, g6, f6, e6, d6, c6, b6, a6,
    h7, g7, f7, e7, d7, c7, b7, a7,
    h8, g8, f8, e8, d8, c8, b8, a8,
};

}