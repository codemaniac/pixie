#include "board.h"
#include "doctest/doctest.h"
#include "movegen.h"
#include "types.h"

using namespace tejas;

TEST_SUITE("Movegen") {

    TEST_CASE("IsSquareAttacked") {
        board::Board board;
        board.SetStartpos();

        REQUIRE(movegen::IsSquareAttacked(board, Square::E3, Color::WHITE));
    }
}
