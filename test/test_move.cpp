#include "doctest/doctest.h"
#include "move.h"
#include "types.h"

using namespace tejas;

TEST_SUITE("Move") {

    TEST_CASE("Move::All") {

        SUBCASE("Default") {
            const move::Move m;
            CHECK(m.getPiece() == Piece::NO_PIECE);
            CHECK(m.getFrom() == Square::NO_SQ);
            CHECK(m.getTo() == Square::NO_SQ);
            CHECK(m.getCaptured() == Piece::NO_PIECE);
            CHECK(m.getFlag() == move::MoveFlag::MOVE_QUIET);
            CHECK(m.getScore() == 0);
        }

        SUBCASE("Comparison") {
            move::Move e2e4 = move::Move(Piece::WHITE_PAWN, Square::E2, Square::E4, Piece::NO_PIECE,
                                         move::MoveFlag::MOVE_QUIET_PAWN_DBL_PUSH);

            CHECK(e2e4.getPiece() == Piece::WHITE_PAWN);
            CHECK(e2e4.getFrom() == Square::E2);
            CHECK(e2e4.getTo() == Square::E4);
            CHECK(e2e4.getCaptured() == Piece::NO_PIECE);
            CHECK(e2e4.getFlag() == move::MoveFlag::MOVE_QUIET_PAWN_DBL_PUSH);
            CHECK(e2e4.getScore() == 0);

            const move::Move e2e4_again =
              move::Move(Piece::WHITE_PAWN, Square::E2, Square::E4, Piece::NO_PIECE,
                         move::MoveFlag::MOVE_QUIET_PAWN_DBL_PUSH);

            CHECK(e2e4 == e2e4_again);
        }
    }
}
