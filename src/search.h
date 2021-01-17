#include "evaluation.h"
#include "moveGenerator.h"

namespace AdiChess {
    
class Search {
public:
    Search(Board &board_);
    int negamax(int depth);
    int quiesce(int alpha, int beta);
    Move getPrincipalMove() const;
    int negamax(int depth, int alpha, int beta);
private:
    Board &board;
    Move principalMove;
};

}