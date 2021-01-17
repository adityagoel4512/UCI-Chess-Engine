#include "search.h"
#include <iostream>

namespace AdiChess {

    Search::Search(Board &board_): board{board_}, principalMove{Move(0, 0, 0)} {}
    
    int Search::negamax(int depth) {
        std::cout << board.getCurrentPlayer() << '\n';
        int alpha = INT32_MIN;
        int beta = INT32_MAX;
        MoveGeneration::MoveGenerator moveGen(board);
        // move ordering
        int value = INT32_MIN;
        for (auto const &move : moveGen) {
            if (board.legalMove(move)) {
                board.makeMove(move);
                auto moveScore = -negamax(depth-1, -beta, -alpha);
                if (moveScore > value) {
                    principalMove = move;
                    value = moveScore;
                }
                board.unmakeMove(move);
            }
        }
        return value;
    }
    int Search::negamax(int depth, int alpha, int beta) {
        if (depth == 0) {
            return quiesce(alpha, beta);
        } 
        MoveGeneration::MoveGenerator moveGen(board);
        // move ordering
        int value = INT32_MIN;
        for (auto const &move : moveGen) {
            if (board.legalMove(move)) {
                board.makeMove(move);
                auto moveScore = -negamax(depth-1, -beta, -alpha);
                value = std::max(value, moveScore);
                board.unmakeMove(move);
                alpha = std::max(alpha, value);
                if (alpha >= beta)
                    return value;
            }
        }
        return value;
    }

    int Search::quiesce(int alpha, int beta) {
        return Evaluation::evaluate(board);
    }

    Move Search::getPrincipalMove() const {
        return principalMove;
    }

}