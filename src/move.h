#include "bitOps.h"

namespace AdiChess {

class Move {

public:
    Move(uint16_t from, uint16_t to, uint8_t flags);

    enum Flag {
        QUIET_MOVE = 0,
        DOUBLE_PAWN_PUSH = 1,
        KING_CASTLE = 2,
        QUEEN_CASTLE = 3,
        CAPTURE = 4,
        EN_PASSANT_CAPTURE = 5,
        KNIGHT_PROMOTION = 6,
        BISHOP_PROMOTION = 7,
        ROOK_PROMOTION = 8,
        QUEEN_PROMOTION = 9,
        KNIGHT_PROMO_CAPTURE = 10,
        BISHOP_PROMO_CAPTURE = 11,
        ROOK_PROMO_CAPTURE = 12,
        QUEEN_PROMO_CAPTURE = 13
    };

    uint64_t getTo() const;
    uint64_t getFrom() const;
    uint64_t getFlag() const;
    bool isCapture() const;
    bool isPromotion() const {
        return getFlag() >= KNIGHT_PROMOTION;
    }

    bool operator==(Move const &other) {
        return move == other.move;
    }
    bool operator!=(Move const &other) {
        return !(*this == other);
    }


private:

    /* BIT LAYOUT:
    LSB |-- 6 --|-- 6 --|-- 3 --| ... MSB
           From    To      Flag */

    uint16_t move;

};

}