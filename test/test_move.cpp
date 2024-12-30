#include "doctest/doctest.h"
#include "move.h"
#include "types.h"

using namespace tejas;

TEST_SUITE("Move") {

    TEST_CASE("Move::All") {

        SUBCASE("Default") {
            const move::Move m;
            CHECK(m.GetPiece() == Piece::NO_PIECE);
            CHECK(m.GetFrom() == Square::NO_SQ);
            CHECK(m.GetTo() == Square::NO_SQ);
            CHECK(m.GetCaptured() == Piece::NO_PIECE);
            CHECK(m.GetFlag() == move::MoveFlag::MOVE_QUIET);
            CHECK(m.GetScore() == 0);
        }

        SUBCASE("Comparison") {
            move::Move e2e4 = move::Move(Piece::WHITE_PAWN, Square::E2, Square::E4, Piece::NO_PIECE,
                                         move::MoveFlag::MOVE_QUIET_PAWN_DBL_PUSH);

            CHECK(e2e4.GetPiece() == Piece::WHITE_PAWN);
            CHECK(e2e4.GetFrom() == Square::E2);
            CHECK(e2e4.GetTo() == Square::E4);
            CHECK(e2e4.GetCaptured() == Piece::NO_PIECE);
            CHECK(e2e4.GetFlag() == move::MoveFlag::MOVE_QUIET_PAWN_DBL_PUSH);
            CHECK(e2e4.GetScore() == 0);

            const move::Move e2e4_again =
              move::Move(Piece::WHITE_PAWN, Square::E2, Square::E4, Piece::NO_PIECE,
                         move::MoveFlag::MOVE_QUIET_PAWN_DBL_PUSH);

            CHECK(e2e4 == e2e4_again);
        }
    }
}
