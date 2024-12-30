#include "board.h"
#include "doctest/doctest.h"
#include "types.h"

using namespace tejas;

TEST_SUITE("Board") {
    TEST_CASE("Board::getPieceCount") {
        board::Board board;
        board.setStartpos();

        CHECK(board.getPieceCount(Piece::WHITE_KING) == 1);
        CHECK(board.getPieceCount(Piece::WHITE_QUEEN) == 1);
        CHECK(board.getPieceCount(Piece::WHITE_ROOK) == 2);
        CHECK(board.getPieceCount(Piece::WHITE_BISHOP) == 2);
        CHECK(board.getPieceCount(Piece::WHITE_KNIGHT) == 2);
        CHECK(board.getPieceCount(Piece::WHITE_PAWN) == 8);
        CHECK(board.getPieceCount(Piece::WHITE_ROOK) == 2);

        CHECK(board.getPieceCount(Piece::BLACK_KING) == 1);
        CHECK(board.getPieceCount(Piece::BLACK_QUEEN) == 1);
        CHECK(board.getPieceCount(Piece::BLACK_ROOK) == 2);
        CHECK(board.getPieceCount(Piece::BLACK_BISHOP) == 2);
        CHECK(board.getPieceCount(Piece::BLACK_KNIGHT) == 2);
        CHECK(board.getPieceCount(Piece::BLACK_PAWN) == 8);
        CHECK(board.getPieceCount(Piece::BLACK_ROOK) == 2);
    }

    TEST_CASE("Board::isValid") {
        board::Board board;

        CHECK(board.isValid() == false);

        board.setStartpos();
        CHECK(board.isValid() == true);

        board.reset();

        board.setPiece(Piece::WHITE_KING, Square::E1);
        CHECK(board.isValid() == false);

        board.setPiece(Piece::BLACK_KING, Square::E8);
        CHECK(board.isValid() == true);

        board.setPiece(Piece::WHITE_PAWN, Square::A1);
        CHECK(board.isValid() == false);

        board.clearPiece(Piece::WHITE_PAWN, Square::A1);
        CHECK(board.isValid() == true);

        board.setPiece(Piece::WHITE_PAWN, Square::H8);
        CHECK(board.isValid() == false);

        board.clearPiece(Piece::WHITE_PAWN, Square::H8);
        CHECK(board.isValid() == true);

        board.setPiece(Piece::BLACK_PAWN, Square::A8);
        CHECK(board.isValid() == false);

        board.clearPiece(Piece::BLACK_PAWN, Square::A8);
        CHECK(board.isValid() == true);

        board.setPiece(Piece::BLACK_PAWN, Square::H1);
        CHECK(board.isValid() == false);

        board.clearPiece(Piece::BLACK_PAWN, Square::H1);
        CHECK(board.isValid() == true);
    }

    TEST_CASE("Board::movePiece and Board::undoMovePiece") {
        board::Board board;

        SUBCASE("Quite Move") {
            board.setStartpos();

            board.movePiece(Piece::WHITE_PAWN, Square::E2, Square::E4);
            CHECK(board.getPiece(Square::E2) == Piece::NO_PIECE);
            CHECK(board.getPiece(Square::E4) == Piece::WHITE_PAWN);

            board.undoMovePiece(Piece::WHITE_PAWN, Square::E2, Square::E4);
            CHECK(board.getPiece(Square::E4) == Piece::NO_PIECE);
            CHECK(board.getPiece(Square::E2) == Piece::WHITE_PAWN);

            board.movePiece(Piece::WHITE_KNIGHT, Square::G1, Square::F3);
            CHECK(board.getPiece(Square::G1) == Piece::NO_PIECE);
            CHECK(board.getPiece(Square::F3) == Piece::WHITE_KNIGHT);

            board.undoMovePiece(Piece::WHITE_KNIGHT, Square::G1, Square::F3);
            CHECK(board.getPiece(Square::G1) == Piece::WHITE_KNIGHT);
            CHECK(board.getPiece(Square::F3) == Piece::NO_PIECE);
        }

        SUBCASE("Capture + Promotion Move") {
            board.reset();
            board.setPiece(Piece::WHITE_KING, Square::E1);
            board.setPiece(Piece::BLACK_KING, Square::E8);
            board.setPiece(Piece::WHITE_PAWN, Square::A7);
            board.setPiece(Piece::BLACK_QUEEN, Square::B8);
            CHECK(board.isValid());

            board.movePiece(Piece::WHITE_PAWN, Square::A7, Square::B8, true, true,
                            Piece::WHITE_QUEEN);
            CHECK(board.getPiece(Square::E1) == Piece::WHITE_KING);
            CHECK(board.getPiece(Square::E8) == Piece::BLACK_KING);
            CHECK(board.getPiece(Square::A7) == Piece::NO_PIECE);
            CHECK(board.getPiece(Square::B8) == Piece::WHITE_QUEEN);
            CHECK(board.getPieceCount(WHITE_PAWN) == 0);
            CHECK(board.getPieceCount(WHITE_QUEEN) == 1);
            CHECK(board.getPieceCount(BLACK_QUEEN) == 0);

            board.undoMovePiece(Piece::WHITE_PAWN, Square::A7, Square::B8, true, Piece::BLACK_QUEEN,
                                true, Piece::WHITE_QUEEN);
            CHECK(board.getPiece(Square::E1) == Piece::WHITE_KING);
            CHECK(board.getPiece(Square::E8) == Piece::BLACK_KING);
            CHECK(board.getPiece(Square::A7) == Piece::WHITE_PAWN);
            CHECK(board.getPiece(Square::B8) == Piece::BLACK_QUEEN);
            CHECK(board.getPieceCount(WHITE_PAWN) == 1);
            CHECK(board.getPieceCount(WHITE_QUEEN) == 0);
            CHECK(board.getPieceCount(BLACK_QUEEN) == 1);
        }
    }
}
