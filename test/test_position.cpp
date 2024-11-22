#include "../src/include/constants.h"
#include "../src/include/fen.h"
#include "../src/include/move.h"
#include "../src/include/position.h"
#include "lib/doctest.h"
#include <memory>

TEST_SUITE("Position") {

    TEST_CASE("Position clone") {
        Position position = Position();
        fen_to_position("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
                        &position);

        Position position_clone = Position(position);

        CHECK(std::addressof(position) != std::addressof(position_clone));
        CHECK(position.get_hash() == position_clone.get_hash());

        position.move_do("e2a6");
        CHECK(position.get_hash() != position_clone.get_hash());

        position.move_undo();
        CHECK(position.get_hash() == position_clone.get_hash());
    }

    TEST_CASE("Position::is_valid") {
        Position position = Position();

        CHECK(position.is_valid() == false);

        position.set_piece(WHITE_KING, E1);
        CHECK(position.is_valid() == false);

        position.set_piece(BLACK_KING, E8);
        CHECK(position.is_valid() == true);

        position.set_piece(WHITE_PAWN, A1);
        CHECK(position.is_valid() == false);
        position.clear_piece(WHITE_PAWN, A1);
        CHECK(position.is_valid() == true);

        position.set_piece(WHITE_PAWN, H8);
        CHECK(position.is_valid() == false);
        position.clear_piece(WHITE_PAWN, H8);
        CHECK(position.is_valid() == true);

        position.set_piece(BLACK_PAWN, A1);
        CHECK(position.is_valid() == false);
        position.clear_piece(BLACK_PAWN, A1);
        CHECK(position.is_valid() == true);

        position.set_piece(BLACK_PAWN, H8);
        CHECK(position.is_valid() == false);
        position.clear_piece(BLACK_PAWN, H8);
        CHECK(position.is_valid() == true);
    }

    TEST_CASE("Position::is_in_check") {
        Position position = Position();

        position.set_start_pos();
        CHECK(position.is_in_check() == false);
    }

    TEST_CASE("Position::is_repeated") {
        Position position = Position();
        position.set_start_pos();

        const uint64_t startpos_hash = position.get_hash();

        Move m;

        m = Move(G1, F3, MOVE_QUIET, NO_PIECE, 0);
        position.move_do(m);
        CHECK(position.is_repeated() == false);
        CHECK(position.get_active_color() == BLACK);

        m = Move(G8, F6, MOVE_QUIET, NO_PIECE, 0);
        position.move_do(m);
        CHECK(position.is_repeated() == false);
        CHECK(position.get_active_color() == WHITE);

        m = Move(F3, G1, MOVE_QUIET, NO_PIECE, 0);
        position.move_do(m);
        CHECK(position.is_repeated() == false);
        CHECK(position.get_active_color() == BLACK);

        m = Move(F6, G8, MOVE_QUIET, NO_PIECE, 0);
        position.move_do(m);
        CHECK(position.is_repeated() == true);
        CHECK(position.get_active_color() == WHITE);

        m = Move(G1, F3, MOVE_QUIET, NO_PIECE, 0);
        position.move_do(m);
        CHECK(position.is_repeated() == true);
        CHECK(position.get_active_color() == BLACK);

        m = Move(G8, F6, MOVE_QUIET, NO_PIECE, 0);
        position.move_do(m);
        CHECK(position.is_repeated() == true);
        CHECK(position.get_active_color() == WHITE);

        m = Move(F3, G1, MOVE_QUIET, NO_PIECE, 0);
        position.move_do(m);
        CHECK(position.is_repeated() == true);
        CHECK(position.get_active_color() == BLACK);

        m = Move(F6, G8, MOVE_QUIET, NO_PIECE, 0);
        position.move_do(m);
        CHECK(position.is_repeated() == true);
        CHECK(position.get_active_color() == WHITE);

        CHECK(position.get_hash() == startpos_hash);
    }
}
