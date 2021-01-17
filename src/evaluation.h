#include "board.h"

using namespace AdiChess;

namespace Evaluation {
    
    enum Phase {
        OPENING=0, ENDGAME=1, NUM_PHASES=2, ALL_PHASES
    };

    int evaluate(Board const &board);
    int computePhase(Board const &board);
    int evaluate(Board const &board, Phase const &phase);
    
};