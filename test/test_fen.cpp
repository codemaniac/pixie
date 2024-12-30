#include "../src/include/fen.h"
#include "../src/include/position.h"
#include "lib/doctest.h"

TEST_SUITE("FEN") {

    TEST_CASE("fen_to_position (full = false)") {
        Position position = Position();
        fen_to_position("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - \"1/2-1/2\"",
                        &position, false);
        CHECK(position.get_half_move_clock() == 0);
        CHECK(position.get_full_move_number() == 1);

        Position startpos = Position();
        startpos.set_start_pos();

        CHECK(position.get_hash() == startpos.get_hash());
    }

    TEST_CASE("fen_to_position (full = true)") {
        Position position = Position();
        fen_to_position("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", &position);
        CHECK(position.get_half_move_clock() == 0);
        CHECK(position.get_full_move_number() == 1);

        Position startpos = Position();
        startpos.set_start_pos();

        CHECK(position.get_hash() == startpos.get_hash());
    }
}
