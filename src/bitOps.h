#pragma once

#include <string>

namespace Utility {

    constexpr uint64_t popCnt(uint64_t bitString) {
        return __builtin_popcountll(bitString);
    }

    // Intrinsic has undefined behaviour if bitString is zero
    constexpr uint64_t bitScanForward(uint64_t bitString) {
        if (!bitString)
            return -1;
        return __builtin_ctzll(bitString);
    }

    constexpr uint64_t bitScanReverse(uint64_t bitString) {
        if (!bitString)
            return -1;
        return 63 - __builtin_clzll(bitString);
    }
    
    constexpr uint64_t getRow(uint64_t bitString) {
        return bitString / 8ULL;
    }

    constexpr uint64_t getCol(uint64_t bitString) {
        return bitString % 8ULL;
    }

    template <typename UINT>
    constexpr bool checkBit(UINT bitString, int n) {
        return bitString & (1ULL << n);
    }

    template <typename UINT>
    constexpr void setBit(UINT &bitString, int n) {        
        bitString |= (static_cast<UINT>(1) << n);
    }

    template <typename UINT>
    constexpr void clearBit(UINT &bitString, int n) {        
        bitString &= ~(static_cast<UINT>(1) << n);
    }

    constexpr int flattenCoordinates(int x, int y) {
        return 63-((x << 3) + y);  
    }

    inline void printGrid(uint64_t bitString, std::ostream &os) {
        os << std::string("\n+---+---+---+---+---+---+---+---+\n");
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                os << std::string("| ") << std::to_string(checkBit(bitString, flattenCoordinates(i, j))) << std::string(" ");
            }
            os << std::string("|\n+---+---+---+---+---+---+---+---+\n");
        }

    }


};