#include "board.h"
#include <vector>
using namespace AdiChess;

namespace MoveGeneration {

enum class GenType {
    LEGAL,
    PSEUDO_LEGAL
};

class MoveGenerator {
    using const_iterator = std::vector<AdiChess::Move>::const_iterator;
    using iterator = std::vector<AdiChess::Move>::iterator;
public:
    explicit MoveGenerator(AdiChess::Board const &);
    size_t size() const {
        return moves.size();
    }

    const_iterator cbegin() const {
        return moves.cbegin();
    }

    iterator begin() {
        return moves.begin();
    }

    const_iterator cend() const {
        return moves.cend();
    }

    iterator end() {
        return moves.end();
    }
private:
    std::vector<AdiChess::Move> moves;
    const AdiChess::Board board;
    void generateLegalMoves();
    template<Piece::Type > void generateMoves();
    uint64_t getAttackMap(uint64_t position, uint64_t oppositionOccupied, Piece::Type const pieceType);
    template<Piece::Type pieceType> void generateAttackMoves(uint64_t position);
    template<Side side> void generatePawnPushMove(uint64_t position, uint64_t freePositions);
    uint64_t promotionRank[2] = {rank8, rank1};
};

}