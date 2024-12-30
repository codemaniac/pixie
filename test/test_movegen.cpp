#include "board.h"
#include "doctest/doctest.h"
#include "movegen.h"
#include "types.h"

using namespace tejas;

TEST_SUITE("Movegen") {

    TEST_CASE("isSquareAttacked") {
        board::Board board;
        board.setStartpos();

        REQUIRE(movegen::isSquareAttacked(board, Square::E3, Color::WHITE));
    }
}
