#include "../src/include/constants.h"
#include "../src/include/move.h"
#include "../src/include/position.h"
#include "lib/doctest.h"


TEST_SUITE("Position") {
    TEST_CASE("testing position repeat") {
        Position position = Position();

        position_init();

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
