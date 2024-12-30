#include "board.h"
#include "doctest/doctest.h"
#include "types.h"

using namespace tejas;

TEST_SUITE("Board") {
    TEST_CASE("Board::get_piece_count") {
        board::Board board;
        board.SetStartpos();

        CHECK(board.GetPieceCount(Piece::WHITE_KING) == 1);
        CHECK(board.GetPieceCount(Piece::WHITE_QUEEN) == 1);
        CHECK(board.GetPieceCount(Piece::WHITE_ROOK) == 2);
        CHECK(board.GetPieceCount(Piece::WHITE_BISHOP) == 2);
        CHECK(board.GetPieceCount(Piece::WHITE_KNIGHT) == 2);
        CHECK(board.GetPieceCount(Piece::WHITE_PAWN) == 8);
        CHECK(board.GetPieceCount(Piece::WHITE_ROOK) == 2);

        CHECK(board.GetPieceCount(Piece::BLACK_KING) == 1);
        CHECK(board.GetPieceCount(Piece::BLACK_QUEEN) == 1);
        CHECK(board.GetPieceCount(Piece::BLACK_ROOK) == 2);
        CHECK(board.GetPieceCount(Piece::BLACK_BISHOP) == 2);
        CHECK(board.GetPieceCount(Piece::BLACK_KNIGHT) == 2);
        CHECK(board.GetPieceCount(Piece::BLACK_PAWN) == 8);
        CHECK(board.GetPieceCount(Piece::BLACK_ROOK) == 2);
    }

    TEST_CASE("Board::is_valid") {
        board::Board board;

        CHECK(board.IsValid() == false);

        board.SetStartpos();
        CHECK(board.IsValid() == true);

        board.Reset();

        board.SetPiece(Piece::WHITE_KING, Square::E1);
        CHECK(board.IsValid() == false);

        board.SetPiece(Piece::BLACK_KING, Square::E8);
        CHECK(board.IsValid() == true);

        board.SetPiece(Piece::WHITE_PAWN, Square::A1);
        CHECK(board.IsValid() == false);

        board.ClearPiece(Piece::WHITE_PAWN, Square::A1);
        CHECK(board.IsValid() == true);

        board.SetPiece(Piece::WHITE_PAWN, Square::H8);
        CHECK(board.IsValid() == false);

        board.ClearPiece(Piece::WHITE_PAWN, Square::H8);
        CHECK(board.IsValid() == true);

        board.SetPiece(Piece::BLACK_PAWN, Square::A8);
        CHECK(board.IsValid() == false);

        board.ClearPiece(Piece::BLACK_PAWN, Square::A8);
        CHECK(board.IsValid() == true);

        board.SetPiece(Piece::BLACK_PAWN, Square::H1);
        CHECK(board.IsValid() == false);

        board.ClearPiece(Piece::BLACK_PAWN, Square::H1);
        CHECK(board.IsValid() == true);
    }

    TEST_CASE("Board::move_piece and Board::undo_move_piece") {
        board::Board board;

        SUBCASE("Quite Move") {
            board.SetStartpos();

            board.MovePiece(Piece::WHITE_PAWN, Square::E2, Square::E4, false, false,
                            Piece::NO_PIECE);
            CHECK(board.GetPiece(Square::E2) == Piece::NO_PIECE);
            CHECK(board.GetPiece(Square::E4) == Piece::WHITE_PAWN);

            board.UndoMovePiece(Piece::WHITE_PAWN, Square::E2, Square::E4, false, Piece::NO_PIECE,
                                false, Piece::NO_PIECE);
            CHECK(board.GetPiece(Square::E4) == Piece::NO_PIECE);
            CHECK(board.GetPiece(Square::E2) == Piece::WHITE_PAWN);

            board.MovePiece(Piece::WHITE_KNIGHT, Square::G1, Square::F3, false, false,
                            Piece::NO_PIECE);
            CHECK(board.GetPiece(Square::G1) == Piece::NO_PIECE);
            CHECK(board.GetPiece(Square::F3) == Piece::WHITE_KNIGHT);

            board.UndoMovePiece(Piece::WHITE_KNIGHT, Square::G1, Square::F3, false, Piece::NO_PIECE,
                                false, Piece::NO_PIECE);
            CHECK(board.GetPiece(Square::G1) == Piece::WHITE_KNIGHT);
            CHECK(board.GetPiece(Square::F3) == Piece::NO_PIECE);
        }

        SUBCASE("Capture + Promotion Move") {
            board.Reset();
            board.SetPiece(Piece::WHITE_KING, Square::E1);
            board.SetPiece(Piece::BLACK_KING, Square::E8);
            board.SetPiece(Piece::WHITE_PAWN, Square::A7);
            board.SetPiece(Piece::BLACK_QUEEN, Square::B8);
            CHECK(board.IsValid());

            board.MovePiece(Piece::WHITE_PAWN, Square::A7, Square::B8, true, true,
                            Piece::WHITE_QUEEN);
            CHECK(board.GetPiece(Square::E1) == Piece::WHITE_KING);
            CHECK(board.GetPiece(Square::E8) == Piece::BLACK_KING);
            CHECK(board.GetPiece(Square::A7) == Piece::NO_PIECE);
            CHECK(board.GetPiece(Square::B8) == Piece::WHITE_QUEEN);
            CHECK(board.GetPieceCount(WHITE_PAWN) == 0);
            CHECK(board.GetPieceCount(WHITE_QUEEN) == 1);
            CHECK(board.GetPieceCount(BLACK_QUEEN) == 0);

            board.UndoMovePiece(Piece::WHITE_PAWN, Square::A7, Square::B8, true, Piece::BLACK_QUEEN,
                                true, Piece::WHITE_QUEEN);
            CHECK(board.GetPiece(Square::E1) == Piece::WHITE_KING);
            CHECK(board.GetPiece(Square::E8) == Piece::BLACK_KING);
            CHECK(board.GetPiece(Square::A7) == Piece::WHITE_PAWN);
            CHECK(board.GetPiece(Square::B8) == Piece::BLACK_QUEEN);
            CHECK(board.GetPieceCount(WHITE_PAWN) == 1);
            CHECK(board.GetPieceCount(WHITE_QUEEN) == 0);
            CHECK(board.GetPieceCount(BLACK_QUEEN) == 1);
        }
    }
}
