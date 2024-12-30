#include "doctest/doctest.h"
#include "move.h"
#include "types.h"

using namespace tejas;

TEST_SUITE("Move") {

    TEST_CASE("Move::All") {

        SUBCASE("Default") {
            const move::Move m;
            CHECK(m.get_piece() == Piece::NO_PIECE);
            CHECK(m.get_from() == Square::NO_SQ);
            CHECK(m.get_to() == Square::NO_SQ);
            CHECK(m.get_captured() == Piece::NO_PIECE);
            CHECK(m.get_flag() == move::MoveFlag::MOVE_QUIET);
            CHECK(m.get_score() == 0);
        }

        SUBCASE("Comparison") {
            move::Move e2e4 = move::Move(Piece::WHITE_PAWN, Square::E2, Square::E4, Piece::NO_PIECE,
                                         move::MoveFlag::MOVE_QUIET_PAWN_DBL_PUSH);

            CHECK(e2e4.get_piece() == Piece::WHITE_PAWN);
            CHECK(e2e4.get_from() == Square::E2);
            CHECK(e2e4.get_to() == Square::E4);
            CHECK(e2e4.get_captured() == Piece::NO_PIECE);
            CHECK(e2e4.get_flag() == move::MoveFlag::MOVE_QUIET_PAWN_DBL_PUSH);
            CHECK(e2e4.get_score() == 0);

            const move::Move e2e4_again =
              move::Move(Piece::WHITE_PAWN, Square::E2, Square::E4, Piece::NO_PIECE,
                         move::MoveFlag::MOVE_QUIET_PAWN_DBL_PUSH);

            CHECK(e2e4 == e2e4_again);
        }
    }
}
