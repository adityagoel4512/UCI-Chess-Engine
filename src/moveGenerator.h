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
    template<Piece::Type > void generateMoves();
    template<Piece::Type pieceType> void generateAttackMoves(uint64_t position);
    template<Side side> void generatePawnPushMove(uint64_t position, uint64_t freePositions);

    uint64_t getAttackMap(uint64_t position, uint64_t oppositionOccupied, Piece::Type const pieceType);
    void generateLegalMoves();

    const Board board;
    const Side currentPlayer;
    uint64_t promotionRank[2] = {rank8, rank1};
    std::vector<Move> moves;
};

}