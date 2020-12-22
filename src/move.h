#include "bitOps.h"

class Move {

public:
    Move(uint64_t from, uint64_t to, uint64_t flags);
    uint64_t getTo() const;
    uint64_t getFrom() const;
    uint64_t getFlags() const;
    bool isCapture() const;
    bool operator==(Move const &other) {
        return move == other.move;
    }
    bool operator!=(Move const &other) {
        return !(*this == other);
    }


private:
    unsigned short move;

};