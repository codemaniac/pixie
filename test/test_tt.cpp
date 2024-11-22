#include "../src/include/constants.h"
#include "../src/include/fen.h"
#include "../src/include/move.h"
#include "../src/include/position.h"
#include "../src/include/tt.h"
#include "lib/doctest.h"
#include <string.h>

TEST_SUITE("Transposition Tables") {
    TEST_CASE("TT store and probe") {
        Position           position = Position();
        TranspositionTable table    = TranspositionTable(2);

        TTData ttdata;
        Move   m;
        bool   tthit = false;

        CHECK(table.get_size() > 0);

        position_init();

        position.set_start_pos();

        m = Move(E2, E4, MOVE_QUIET_PAWN_DBL_PUSH, NO_PIECE, 0);

        table.store(position, 1, EXACT, 0, m, 1);

        tthit = table.probe(position, &ttdata, 1);

        CHECK(tthit == true);
        CHECK(ttdata.depth == 1);
        CHECK(ttdata.flag == EXACT);
        CHECK(ttdata.value == 0);
        CHECK(ttdata.move == m);
        CHECK(ttdata.is_valid == true);

        tthit = false;

        std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
        fen_to_position(fen, &position);

        m = Move(E2, A6, MOVE_CAPTURE, BLACK_BISHOP,
                 MOVE_SCORE_MVV_LVA[MOVE_SCORE_MVV_LVA_IDX(BISHOP, BISHOP)]);

        table.store(position, 1, EXACT, 100, m, 1);

        ttdata = TTData();
        tthit  = table.probe(position, &ttdata, 1);

        CHECK(tthit == true);
        CHECK(ttdata.depth == 1);
        CHECK(ttdata.flag == EXACT);
        CHECK(ttdata.value == 100);
        CHECK(ttdata.move == m);
        CHECK(ttdata.move.get_score()
              == MOVE_SCORE_MVV_LVA[MOVE_SCORE_MVV_LVA_IDX(BISHOP, BISHOP)]);
        CHECK(ttdata.is_valid == true);
    }
}
