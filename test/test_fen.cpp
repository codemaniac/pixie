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

        // Test with Kiwipete
        std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -";
        fen::parseFEN(&board, fen);

        REQUIRE(board.isValid());

        REQUIRE(board.getPiece(Square::A1) == Piece::WHITE_ROOK);
        REQUIRE(board.getPiece(Square::B1) == Piece::NO_PIECE);
        REQUIRE(board.getPiece(Square::C1) == Piece::NO_PIECE);
        REQUIRE(board.getPiece(Square::D1) == Piece::NO_PIECE);
        REQUIRE(board.getPiece(Square::E1) == Piece::WHITE_KING);
        REQUIRE(board.getPiece(Square::F1) == Piece::NO_PIECE);
        REQUIRE(board.getPiece(Square::G1) == Piece::NO_PIECE);
        REQUIRE(board.getPiece(Square::H1) == Piece::WHITE_ROOK);

        REQUIRE(board.getPiece(Square::A2) == Piece::WHITE_PAWN);
        REQUIRE(board.getPiece(Square::B2) == Piece::WHITE_PAWN);
        REQUIRE(board.getPiece(Square::C2) == Piece::WHITE_PAWN);
        REQUIRE(board.getPiece(Square::D2) == Piece::WHITE_BISHOP);
        REQUIRE(board.getPiece(Square::E2) == Piece::WHITE_BISHOP);
        REQUIRE(board.getPiece(Square::F2) == Piece::WHITE_PAWN);
        REQUIRE(board.getPiece(Square::G2) == Piece::WHITE_PAWN);
        REQUIRE(board.getPiece(Square::H2) == Piece::WHITE_PAWN);

        REQUIRE(board.getPiece(Square::A3) == Piece::NO_PIECE);
        REQUIRE(board.getPiece(Square::B3) == Piece::NO_PIECE);
        REQUIRE(board.getPiece(Square::C3) == Piece::WHITE_KNIGHT);
        REQUIRE(board.getPiece(Square::D3) == Piece::NO_PIECE);
        REQUIRE(board.getPiece(Square::E3) == Piece::NO_PIECE);
        REQUIRE(board.getPiece(Square::F3) == Piece::WHITE_QUEEN);
        REQUIRE(board.getPiece(Square::G3) == Piece::NO_PIECE);
        REQUIRE(board.getPiece(Square::H3) == Piece::BLACK_PAWN);

        REQUIRE(board.getPiece(Square::A4) == Piece::NO_PIECE);
        REQUIRE(board.getPiece(Square::B4) == Piece::BLACK_PAWN);
        REQUIRE(board.getPiece(Square::C4) == Piece::NO_PIECE);
        REQUIRE(board.getPiece(Square::D4) == Piece::NO_PIECE);
        REQUIRE(board.getPiece(Square::E4) == Piece::WHITE_PAWN);
        REQUIRE(board.getPiece(Square::F4) == Piece::NO_PIECE);
        REQUIRE(board.getPiece(Square::G4) == Piece::NO_PIECE);
        REQUIRE(board.getPiece(Square::H4) == Piece::NO_PIECE);

        REQUIRE(board.getPiece(Square::A5) == Piece::NO_PIECE);
        REQUIRE(board.getPiece(Square::B5) == Piece::NO_PIECE);
        REQUIRE(board.getPiece(Square::C5) == Piece::NO_PIECE);
        REQUIRE(board.getPiece(Square::D5) == Piece::WHITE_PAWN);
        REQUIRE(board.getPiece(Square::E5) == Piece::WHITE_KNIGHT);
        REQUIRE(board.getPiece(Square::F5) == Piece::NO_PIECE);
        REQUIRE(board.getPiece(Square::G5) == Piece::NO_PIECE);
        REQUIRE(board.getPiece(Square::H5) == Piece::NO_PIECE);

        REQUIRE(board.getPiece(Square::A6) == Piece::BLACK_BISHOP);
        REQUIRE(board.getPiece(Square::B6) == Piece::BLACK_KNIGHT);
        REQUIRE(board.getPiece(Square::C6) == Piece::NO_PIECE);
        REQUIRE(board.getPiece(Square::D6) == Piece::NO_PIECE);
        REQUIRE(board.getPiece(Square::E6) == Piece::BLACK_PAWN);
        REQUIRE(board.getPiece(Square::F6) == Piece::BLACK_KNIGHT);
        REQUIRE(board.getPiece(Square::G6) == Piece::BLACK_PAWN);
        REQUIRE(board.getPiece(Square::H6) == Piece::NO_PIECE);

        REQUIRE(board.getPiece(Square::A7) == Piece::BLACK_PAWN);
        REQUIRE(board.getPiece(Square::B7) == Piece::NO_PIECE);
        REQUIRE(board.getPiece(Square::C7) == Piece::BLACK_PAWN);
        REQUIRE(board.getPiece(Square::D7) == Piece::BLACK_PAWN);
        REQUIRE(board.getPiece(Square::E7) == Piece::BLACK_QUEEN);
        REQUIRE(board.getPiece(Square::F7) == Piece::BLACK_PAWN);
        REQUIRE(board.getPiece(Square::G7) == Piece::BLACK_BISHOP);
        REQUIRE(board.getPiece(Square::H7) == Piece::NO_PIECE);

        REQUIRE(board.getPiece(Square::A8) == Piece::BLACK_ROOK);
        REQUIRE(board.getPiece(Square::B8) == Piece::NO_PIECE);
        REQUIRE(board.getPiece(Square::C8) == Piece::NO_PIECE);
        REQUIRE(board.getPiece(Square::D8) == Piece::NO_PIECE);
        REQUIRE(board.getPiece(Square::E8) == Piece::BLACK_KING);
        REQUIRE(board.getPiece(Square::F8) == Piece::NO_PIECE);
        REQUIRE(board.getPiece(Square::G8) == Piece::NO_PIECE);
        REQUIRE(board.getPiece(Square::H8) == Piece::BLACK_ROOK);

        REQUIRE(board.getActiveColor() == Color::WHITE);
        REQUIRE((board.getCastelingRights() & board::CastleFlag::WKCA) != 0);
        REQUIRE((board.getCastelingRights() & board::CastleFlag::WQCA) != 0);
        REQUIRE((board.getCastelingRights() & board::CastleFlag::BKCA) != 0);
        REQUIRE((board.getCastelingRights() & board::CastleFlag::BQCA) != 0);
        REQUIRE(board.getEnpassantTarget() == Square::NO_SQ);
        REQUIRE(board.getHalfmoveClock() == 0);
        REQUIRE(board.getFullmoveNumber() == 1);
    }
}
