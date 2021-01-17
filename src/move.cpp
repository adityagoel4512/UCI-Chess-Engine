#include "move.h"

namespace AdiChess {

    Move::Move(uint16_t from, uint16_t to, uint8_t flags): move{((flags & 0xF)<<12) | (from<<6) | to} {}

    uint64_t Move::getFrom() const {
        return (move >> 6) & 0x3F;
    }

    uint64_t Move::getTo() const {
        return move & 0x3F;
    }

    uint64_t Move::getFlag() const {
       return (move >> 12) & 0x0F;
    }

    bool Move::isCapture() const {
        auto flags = getFlag();
        return flags == Flag::CAPTURE || flags == Flag::EN_PASSANT_CAPTURE || (flags <= Flag::QUEEN_PROMO_CAPTURE && flags >= Flag::KNIGHT_PROMO_CAPTURE);
    }

    bool Move::isPromotion() const {
        return getFlag() >= KNIGHT_PROMOTION;
    }

}