#include "board.h"
#include "doctest/doctest.h"
#include "fen.h"
#include "move.h"
#include "movegen.h"
#include "types.h"
#include <string>
#include <vector>

using namespace tejas;

TEST_SUITE("Movegen") {

    TEST_CASE("isSquareAttacked") {
        board::Board board;

        std::string fen = "4k3/8/8/4p3/8/8/8/4K3 w - - 0 1";
        fen::parseFEN(&board, fen);

        REQUIRE(movegen::isSquareAttacked(board, Square::D4, Color::BLACK));
        REQUIRE(movegen::isSquareAttacked(board, Square::F4, Color::BLACK));
    }

    TEST_CASE("isInCheck") {
        board::Board board;

        std::string fen = "4k3/8/8/8/8/8/3p4/4K3 w - - 0 1";
        fen::parseFEN(&board, fen);

        REQUIRE(movegen::isInCheck(board));
    }

    TEST_CASE("generatePseudolegalMoves") {
        board::Board board;
        board.setStartpos();

        std::vector<move::Move> moves;
        movegen::generatePseudolegalMoves(&moves, board, movegen::MovegenType::ALL);

        CHECK(moves.size() == 20);
    }
}
