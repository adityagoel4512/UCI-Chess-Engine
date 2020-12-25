#include "board.h"
#include <vector>
using namespace AdiChess;

namespace MoveGeneration {

enum class GenType {
    LEGAL,
    PSEUDO_LEGAL
};

class MoveGenerator {
    using const_iterator = std::vector<Move>::const_iterator;
    using iterator = std::vector<Move>::iterator;
public:
    explicit MoveGenerator(Board const &);
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
    template<Piece::Type> void generateMoves();
    template<Piece::Type> void generateAttackMoves(uint64_t position);
    template<Side> void generatePawnPushMove(uint64_t position, uint64_t freePositions);

    uint64_t getAttackMap(uint64_t position, Piece::Type const pieceType);
    void generateLegalMoves();

    const Board board;
    const Side currentPlayer;
    const uint64_t friendlyOccupied;
    const uint64_t oppositionOccupied;
    const uint64_t promotionRank[2] = {rank8, rank1};
    std::vector<Move> moves;
};

}