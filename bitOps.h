namespace Utility {

    constexpr int flattenCoordinates(int x, int y) {
        return 8 * x + y;  
    }

    template <typename UINT>
    constexpr bool checkBit(UINT bitString, int n) {
        return bitString & (static_cast<UINT>(1) << n);
    }

    template <typename UINT>
    constexpr void setBit(UINT &bitString, int n) {
        bitString |= (static_cast<UINT>(0b1) << n);
    }
};