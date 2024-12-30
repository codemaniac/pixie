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

    TEST_CASE("Board::move_piece and Board::undo_move_piece") {
        board::Board board;

        SUBCASE("Quite Move") {
            board.set_startpos();

            board.move_piece(Piece::WHITE_PAWN, Square::E2, Square::E4, false, false,
                             Piece::NO_PIECE);
            CHECK(board.get_piece(Square::E2) == Piece::NO_PIECE);
            CHECK(board.get_piece(Square::E4) == Piece::WHITE_PAWN);

            board.undo_move_piece(Piece::WHITE_PAWN, Square::E2, Square::E4, false, Piece::NO_PIECE,
                                  false, Piece::NO_PIECE);
            CHECK(board.get_piece(Square::E4) == Piece::NO_PIECE);
            CHECK(board.get_piece(Square::E2) == Piece::WHITE_PAWN);

            board.move_piece(Piece::WHITE_KNIGHT, Square::G1, Square::F3, false, false,
                             Piece::NO_PIECE);
            CHECK(board.get_piece(Square::G1) == Piece::NO_PIECE);
            CHECK(board.get_piece(Square::F3) == Piece::WHITE_KNIGHT);

            board.undo_move_piece(Piece::WHITE_KNIGHT, Square::G1, Square::F3, false,
                                  Piece::NO_PIECE, false, Piece::NO_PIECE);
            CHECK(board.get_piece(Square::G1) == Piece::WHITE_KNIGHT);
            CHECK(board.get_piece(Square::F3) == Piece::NO_PIECE);
        }

        SUBCASE("Capture + Promotion Move") {
            board.reset();
            board.set_piece(Piece::WHITE_KING, Square::E1);
            board.set_piece(Piece::BLACK_KING, Square::E8);
            board.set_piece(Piece::WHITE_PAWN, Square::A7);
            board.set_piece(Piece::BLACK_QUEEN, Square::B8);
            CHECK(board.is_valid());

            board.move_piece(Piece::WHITE_PAWN, Square::A7, Square::B8, true, true,
                             Piece::WHITE_QUEEN);
            CHECK(board.get_piece(Square::E1) == Piece::WHITE_KING);
            CHECK(board.get_piece(Square::E8) == Piece::BLACK_KING);
            CHECK(board.get_piece(Square::A7) == Piece::NO_PIECE);
            CHECK(board.get_piece(Square::B8) == Piece::WHITE_QUEEN);
            CHECK(board.get_piece_count(WHITE_PAWN) == 0);
            CHECK(board.get_piece_count(WHITE_QUEEN) == 1);
            CHECK(board.get_piece_count(BLACK_QUEEN) == 0);

            board.undo_move_piece(Piece::WHITE_PAWN, Square::A7, Square::B8, true,
                                  Piece::BLACK_QUEEN, true, Piece::WHITE_QUEEN);
            CHECK(board.get_piece(Square::E1) == Piece::WHITE_KING);
            CHECK(board.get_piece(Square::E8) == Piece::BLACK_KING);
            CHECK(board.get_piece(Square::A7) == Piece::WHITE_PAWN);
            CHECK(board.get_piece(Square::B8) == Piece::BLACK_QUEEN);
            CHECK(board.get_piece_count(WHITE_PAWN) == 1);
            CHECK(board.get_piece_count(WHITE_QUEEN) == 0);
            CHECK(board.get_piece_count(BLACK_QUEEN) == 1);
        }
    }
}
