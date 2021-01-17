#include "evaluation.h"

namespace Evaluation {
    
    constexpr int MATERIAL_VALUES[2][6] = {
        [Phase::OPENING] = {
            [Piece::Type::K] = 0,
            [Piece::Type::Q] = 2538, 
            [Piece::Type::B] = 825, 
            [Piece::Type::R] = 1276,
            [Piece::Type::N] = 781,
            [Piece::Type::P] = 126
        },
        [Phase::ENDGAME] = {
            [Piece::Type::K] = 0,
            [Piece::Type::Q] = 2682, 
            [Piece::Type::B] = 915, 
            [Piece::Type::R] = 1380,
            [Piece::Type::N] = 854,
            [Piece::Type::P] = 208
        },
    };

    constexpr int PIECE_PHASES[6] = {
        [Piece::Type::K] = 0,
        [Piece::Type::Q] = 4, 
        [Piece::Type::B] = 1, 
        [Piece::Type::R] = 2,
        [Piece::Type::N] = 1,
        [Piece::Type::P] = 0
    };

    int evaluate(Board const &board, Phase const &phase) {
        int materialValue = 0;

        for (Piece::Type pieceType = Piece::Type::K; pieceType < Piece::Type::NUM_PIECES; pieceType = Piece::Type(pieceType+1)) {
            int friendlyPositions = board.getPositions(pieceType, board.getCurrentPlayer());
            int opponentPositions = board.getPositions(pieceType, board.getOpponent());
            int friendlyPopCnt = Utility::popCnt(friendlyPositions);
            int opponentPopCnt = Utility::popCnt(opponentPositions);

            materialValue += MATERIAL_VALUES[phase][pieceType] * (friendlyPopCnt - opponentPopCnt);

            // Bishop synergy bonus
            if (pieceType == Piece::Type::B) {
                if (friendlyPopCnt == 2)
                    materialValue += 100;
                if (opponentPopCnt == 2)
                    materialValue += 100;
            }
        }

        return materialValue;

    }

    int computePhase(Board const &board) {
        static constexpr int TOTAL_PHASE_SUM = PIECE_PHASES[Piece::Type::P] * 16 + PIECE_PHASES[Piece::Type::N]*4 + PIECE_PHASES[Piece::Type::B]*4 + PIECE_PHASES[Piece::Type::R]*4 + PIECE_PHASES[Piece::Type::Q]*2;
        int phase = TOTAL_PHASE_SUM;
        for (Piece::Type pieceType = Piece::Type::K; pieceType < Piece::Type::NUM_PIECES; pieceType = Piece::Type(pieceType+1)) {
            phase -= Utility::popCnt(board.getPositions(pieceType, Side::W));
            phase -= Utility::popCnt(board.getPositions(pieceType, Side::B));
        }
        return (phase * 256 + (TOTAL_PHASE_SUM / 2)) / TOTAL_PHASE_SUM;
    }

    int evaluate(Board const &board) {
        int phase = computePhase(board);
        int opening = evaluate(board, Phase::OPENING);
        int endgame = evaluate(board, Phase::ENDGAME);
        return ((opening * (256 - phase)) + (endgame * phase)) / 256;
    }

}