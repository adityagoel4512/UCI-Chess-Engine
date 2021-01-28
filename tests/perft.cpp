#include "../src/board.h"
#include "../src/moveGenerator.h"
#include "gtest/gtest.h"

using namespace AdiChess;

static uint64_t perft(int depth, AdiChess::Board &board) {
    if (depth == 0) {
        return 1;
    }
    uint64_t nodes = 0;
    MoveGeneration::MoveGenerator moveGenerator(board);

    for (auto const &move : moveGenerator) {

        if (board.legalMove(move)) {
            board.makeMove(move);
            nodes += perft(depth - 1, board);
            board.unmakeMove(move);
        }
    }

    return nodes;
}

TEST(StartPosition, Perft0) {
    Board board;
    ASSERT_EQ(perft(0, board), 1);
}

TEST(StartPosition, Perft1) {
    Board board;
    ASSERT_EQ(perft(1, board), 20);
}

TEST(StartPosition, Perft2) {
    Board board;
    ASSERT_EQ(perft(2, board), 400);
}

TEST(StartPosition, Perft3) {
    Board board;
    ASSERT_EQ(perft(3, board), 8902);
}

TEST(StartPosition, Perft4) {
    Board board;
    ASSERT_EQ(perft(4, board), 197281);
}

TEST(StartPosition, Perft5) {
    Board board;
    ASSERT_EQ(perft(5, board), 4865609);
}

TEST(Position2, Perft1) {
    Board board("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
    ASSERT_EQ(perft(1, board), 44);
}

TEST(Position2, Perft2) {
    Board board("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
    ASSERT_EQ(perft(2, board), 1486);
}

TEST(Position3, Perft1) {
    Board board(
        "rr3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
    ASSERT_EQ(perft(1, board), 6);
}

TEST(Position3, Perft2) {
    Board board(
        "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
    ASSERT_EQ(perft(2, board), 264);
}

TEST(Position3, Perft3) {
    Board board(
        "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
    ASSERT_EQ(perft(3, board), 9467);
}