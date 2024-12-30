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

    TEST_CASE("Board::is_valid") {
        board::Board board;

        CHECK(board.is_valid() == false);

        board.set_startpos();
        CHECK(board.is_valid() == true);

        board.reset();

        board.set_piece(Piece::WHITE_KING, Square::E1);
        CHECK(board.is_valid() == false);

        board.set_piece(Piece::BLACK_KING, Square::E8);
        CHECK(board.is_valid() == true);

        board.set_piece(Piece::WHITE_PAWN, Square::A1);
        CHECK(board.is_valid() == false);

        board.clear_piece(Piece::WHITE_PAWN, Square::A1);
        CHECK(board.is_valid() == true);

        board.set_piece(Piece::WHITE_PAWN, Square::H8);
        CHECK(board.is_valid() == false);

        board.clear_piece(Piece::WHITE_PAWN, Square::H8);
        CHECK(board.is_valid() == true);

        board.set_piece(Piece::BLACK_PAWN, Square::A8);
        CHECK(board.is_valid() == false);

        board.clear_piece(Piece::BLACK_PAWN, Square::A8);
        CHECK(board.is_valid() == true);

        board.set_piece(Piece::BLACK_PAWN, Square::H1);
        CHECK(board.is_valid() == false);

        board.clear_piece(Piece::BLACK_PAWN, Square::H1);
        CHECK(board.is_valid() == true);
    }
}
