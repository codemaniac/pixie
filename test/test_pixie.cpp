#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../src/include/constants.h"
#include "../src/include/fen.h"
#include "../src/include/move.h"
#include "../src/include/perft.h"
#include "../src/include/position.h"
#include "../src/include/threadpool.h"
#include "../src/include/tt.h"
#include "lib/doctest.h"
#include <memory.h>
#include <string.h>

TEST_CASE("testing move generation using perft") {
    std::unique_ptr<Position>   position = std::make_unique<Position>();
    std::unique_ptr<ThreadPool> pool     = std::make_unique<ThreadPool>(2);

    position_init();

    position->set_start_pos();

    const uint64_t nodes = perft_multithreaded(position, 4, pool, false);
    CHECK(nodes == 197281);
}

TEST_CASE("testing Transposition Table store and probe") {
    std::unique_ptr<Position>           position = std::make_unique<Position>();
    std::unique_ptr<TranspositionTable> table    = std::make_unique<TranspositionTable>(2);

    TTData ttdata;
    Move    m;
    bool    tthit = false;

    CHECK(table->get_size() > 0);

    position_init();

    position->set_start_pos();

    m = Move(E2, E4, MOVE_QUIET_PAWN_DBL_PUSH, NO_PIECE, 0);

    table->store(position, 1, EXACT, 0, m);

    tthit = table->probe(position, &ttdata);

    CHECK(tthit == true);
    CHECK(ttdata.depth == 1);
    CHECK(ttdata.flag == EXACT);
    CHECK(ttdata.value == 0);
    CHECK(ttdata.move == m);
    CHECK(ttdata.is_valid == true);

    tthit = false;

    std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    fen_to_position(fen, position);

    m = Move(E2, A6, MOVE_CAPTURE, BLACK_BISHOP,
             MOVE_SCORE_MVV_LVA[MOVE_SCORE_MVV_LVA_IDX(BISHOP, BISHOP)]);

    table->store(position, 1, EXACT, 100, m);

    ttdata = TTData();
    tthit = table->probe(position, &ttdata);

    CHECK(tthit == true);
    CHECK(ttdata.depth == 1);
    CHECK(ttdata.flag == EXACT);
    CHECK(ttdata.value == 100);
    CHECK(ttdata.move == m);
    CHECK(ttdata.is_valid == true);
}

TEST_CASE("testing position repeat") {
    std::unique_ptr<Position> position = std::make_unique<Position>();

    position_init();

    position->set_start_pos();

    const uint64_t startpos_hash = position->get_hash();

    Move m;

    m = Move(G1, F3, MOVE_QUIET, NO_PIECE, 0);
    position->move_do(m);
    CHECK(position->is_repeated() == false);
    CHECK(position->get_active_color() == BLACK);

    m = Move(G8, F6, MOVE_QUIET, NO_PIECE, 0);
    position->move_do(m);
    CHECK(position->is_repeated() == false);
    CHECK(position->get_active_color() == WHITE);

    m = Move(F3, G1, MOVE_QUIET, NO_PIECE, 0);
    position->move_do(m);
    CHECK(position->is_repeated() == false);
    CHECK(position->get_active_color() == BLACK);

    m = Move(F6, G8, MOVE_QUIET, NO_PIECE, 0);
    position->move_do(m);
    CHECK(position->is_repeated() == true);
    CHECK(position->get_active_color() == WHITE);

    m = Move(G1, F3, MOVE_QUIET, NO_PIECE, 0);
    position->move_do(m);
    CHECK(position->is_repeated() == true);
    CHECK(position->get_active_color() == BLACK);

    m = Move(G8, F6, MOVE_QUIET, NO_PIECE, 0);
    position->move_do(m);
    CHECK(position->is_repeated() == true);
    CHECK(position->get_active_color() == WHITE);

    m = Move(F3, G1, MOVE_QUIET, NO_PIECE, 0);
    position->move_do(m);
    CHECK(position->is_repeated() == true);
    CHECK(position->get_active_color() == BLACK);

    m = Move(F6, G8, MOVE_QUIET, NO_PIECE, 0);
    position->move_do(m);
    CHECK(position->is_repeated() == true);
    CHECK(position->get_active_color() == WHITE);

    CHECK(position->get_hash() == startpos_hash);
}
