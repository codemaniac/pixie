#include "board.h"
#include "doctest/doctest.h"
#include "fen.h"
#include "types.h"
#include <string>

using namespace tejas;

TEST_SUITE("FEN") {

    TEST_CASE("fen::parseFEN") {
        board::Board board;

        std::string startpos_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
        fen::parseFEN(&board, startpos_fen);

        board::Board startpos_board;
        startpos_board.setStartpos();

        CHECK(board == startpos_board);

        startpos_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -";
        fen::parseFEN(&board, startpos_fen);

        CHECK(board == startpos_board);

        startpos_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - \"1/2-1/2\"";
        fen::parseFEN(&board, startpos_fen, false);

        CHECK(board == startpos_board);
    }
}
