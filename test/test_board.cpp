#include "board.h"
#include "doctest/doctest.h"
#include "types.h"

using namespace tejas;

TEST_SUITE("Board") {
    TEST_CASE("Board::get_piece_count") {
        board::Board board;
        board.set_startpos();

        CHECK(board.get_piece_count(Piece::WHITE_KING) == 1);
        CHECK(board.get_piece_count(Piece::WHITE_QUEEN) == 1);
        CHECK(board.get_piece_count(Piece::WHITE_ROOK) == 2);
        CHECK(board.get_piece_count(Piece::WHITE_BISHOP) == 2);
        CHECK(board.get_piece_count(Piece::WHITE_KNIGHT) == 2);
        CHECK(board.get_piece_count(Piece::WHITE_PAWN) == 8);
        CHECK(board.get_piece_count(Piece::WHITE_ROOK) == 2);

        CHECK(board.get_piece_count(Piece::BLACK_KING) == 1);
        CHECK(board.get_piece_count(Piece::BLACK_QUEEN) == 1);
        CHECK(board.get_piece_count(Piece::BLACK_ROOK) == 2);
        CHECK(board.get_piece_count(Piece::BLACK_BISHOP) == 2);
        CHECK(board.get_piece_count(Piece::BLACK_KNIGHT) == 2);
        CHECK(board.get_piece_count(Piece::BLACK_PAWN) == 8);
        CHECK(board.get_piece_count(Piece::BLACK_ROOK) == 2);
    }
}
