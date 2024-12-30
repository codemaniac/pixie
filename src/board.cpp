#include "board.h"
#include "utils.h"

#ifdef DEBUG
    #include <cassert>
    #include <iostream>
#endif

namespace tejas {

    namespace board {

        Board::Board() { Reset(); }

        Board::~Board() {}

        void Board::Reset() {
            for (auto& bb : bitboards)
            {
                bb = 0ULL;
            }

            for (auto& p : pieces)
            {
                p = Piece::NO_PIECE;
            }

            bitboards[Piece::NO_PIECE] = 0xFFFFFFFFFFFFFFFF;
        }

        void Board::SetPiece(const Piece piece, const Square square) {
            const BitBoard bit          = 1ULL << square;
            const BitBoard bit_inverted = ~(bit);
            bitboards[piece] |= bit;
            bitboards[Piece::NO_PIECE] &= bit_inverted;
            bitboards[PieceColorIndex(piece)] |= bit;
            pieces[square] = piece;
        }

        void Board::ClearPiece(const Piece piece, const Square square) {
            const BitBoard bit          = 1ULL << square;
            const BitBoard bit_inverted = ~(bit);
            bitboards[piece] &= bit_inverted;
            bitboards[Piece::NO_PIECE] |= bit;
            bitboards[PieceColorIndex(piece)] &= bit_inverted;
            pieces[square] = Piece::NO_PIECE;
        }

        void Board::SetStartpos() {
            Reset();

            SetPiece(Piece::WHITE_ROOK, Square::A1);
            SetPiece(Piece::WHITE_KNIGHT, Square::B1);
            SetPiece(Piece::WHITE_BISHOP, Square::C1);
            SetPiece(Piece::WHITE_QUEEN, Square::D1);
            SetPiece(Piece::WHITE_KING, Square::E1);
            SetPiece(Piece::WHITE_BISHOP, Square::F1);
            SetPiece(Piece::WHITE_KNIGHT, Square::G1);
            SetPiece(Piece::WHITE_ROOK, Square::H1);

            SetPiece(Piece::WHITE_PAWN, Square::A2);
            SetPiece(Piece::WHITE_PAWN, Square::B2);
            SetPiece(Piece::WHITE_PAWN, Square::C2);
            SetPiece(Piece::WHITE_PAWN, Square::D2);
            SetPiece(Piece::WHITE_PAWN, Square::E2);
            SetPiece(Piece::WHITE_PAWN, Square::F2);
            SetPiece(Piece::WHITE_PAWN, Square::G2);
            SetPiece(Piece::WHITE_PAWN, Square::H2);

            SetPiece(Piece::BLACK_ROOK, Square::A8);
            SetPiece(Piece::BLACK_KNIGHT, Square::B8);
            SetPiece(Piece::BLACK_BISHOP, Square::C8);
            SetPiece(Piece::BLACK_QUEEN, Square::D8);
            SetPiece(Piece::BLACK_KING, Square::E8);
            SetPiece(Piece::BLACK_BISHOP, Square::F8);
            SetPiece(Piece::BLACK_KNIGHT, Square::G8);
            SetPiece(Piece::BLACK_ROOK, Square::H8);

            SetPiece(Piece::BLACK_PAWN, Square::A7);
            SetPiece(Piece::BLACK_PAWN, Square::B7);
            SetPiece(Piece::BLACK_PAWN, Square::C7);
            SetPiece(Piece::BLACK_PAWN, Square::D7);
            SetPiece(Piece::BLACK_PAWN, Square::E7);
            SetPiece(Piece::BLACK_PAWN, Square::F7);
            SetPiece(Piece::BLACK_PAWN, Square::G7);
            SetPiece(Piece::BLACK_PAWN, Square::H7);
        }

        void Board::MovePiece(const Piece  piece,
                              const Square from,
                              const Square to,
                              const bool   is_capture,
                              const bool   is_promotion,
                              const Piece  promoted) {

            ClearPiece(piece, from);

            if (is_capture)
            {
                const Piece captured = GetPiece(to);
#ifdef DEBUG
                assert(captured != Piece::NO_PIECE);
                assert(PieceColorOf(piece) != PieceColorOf(captured));
                assert(PieceTypeOf(captured) != PieceType::KING);
#endif
                ClearPiece(captured, to);
            }

            if (is_promotion)
            {
#ifdef DEBUG
                assert(PieceTypeOf(piece) == PieceType::PAWN);
                if (PieceColorOf(piece) == Color::WHITE)
                {
                    assert(SQ2Rank(from) == Rank::RANK_7);
                    assert(SQ2Rank(to) == Rank::RANK_8);
                }
                else
                {
                    assert(SQ2Rank(from) == Rank::RANK_2);
                    assert(SQ2Rank(to) == Rank::RANK_1);
                }
                assert(promoted != Piece::NO_PIECE);
                assert(PieceTypeOf(promoted) != PieceType::PAWN);
                assert(PieceTypeOf(promoted) != PieceType::KING);
                assert(PieceColorOf(piece) == PieceColorOf(promoted));
#endif
                SetPiece(promoted, to);
            }
            else
            {
                SetPiece(piece, to);
            }
        }

        void Board::UndoMovePiece(const Piece  piece,
                                  const Square from,
                                  const Square to,
                                  const bool   is_capture,
                                  const Piece  captured,
                                  const bool   is_promotion,
                                  const Piece  promoted) {

            if (is_promotion)
            {
#ifdef DEBUG
                assert(PieceTypeOf(piece) == PieceType::PAWN);
                if (PieceColorOf(piece) == Color::WHITE)
                {
                    assert(SQ2Rank(from) == Rank::RANK_7);
                    assert(SQ2Rank(to) == Rank::RANK_8);
                }
                else
                {
                    assert(SQ2Rank(from) == Rank::RANK_2);
                    assert(SQ2Rank(to) == Rank::RANK_1);
                }
                assert(promoted != Piece::NO_PIECE);
                assert(PieceTypeOf(promoted) != PieceType::PAWN);
                assert(PieceTypeOf(promoted) != PieceType::KING);
                assert(PieceColorOf(piece) == PieceColorOf(promoted));
#endif
                ClearPiece(promoted, to);
            }
            else
            {
                ClearPiece(piece, to);
            }

            if (is_capture)
            {
#ifdef DEBUG
                assert(captured != Piece::NO_PIECE);
                assert(PieceColorOf(piece) != PieceColorOf(captured));
                assert(PieceTypeOf(captured) != PieceType::KING);
#endif
                SetPiece(captured, to);
            }

            SetPiece(piece, from);
        }

        BitBoard Board::GetBitboard(const u8 index) const { return bitboards[index]; }

        Piece Board::GetPiece(const Square square) const { return pieces[square]; }

        u8 Board::GetPieceCount(const Piece piece) const {
            return utils::BitCount1s(bitboards[piece]);
        }

        bool Board::IsValid() const {
            const bool check = (GetPieceCount(Piece::WHITE_KING) == 1)
                            && (GetPieceCount(Piece::BLACK_KING) == 1)
                            && (!(bitboards[Piece::WHITE_PAWN] & MASK_RANK_1))
                            && (!(bitboards[Piece::WHITE_PAWN] & MASK_RANK_8))
                            && (!(bitboards[Piece::BLACK_PAWN] & MASK_RANK_1))
                            && (!(bitboards[Piece::BLACK_PAWN] & MASK_RANK_8));
            return check;
        }

#ifdef DEBUG
        void Board::Display() const {
            const char* pieces_str = ".PNBRQKXXpnbrqk";
            for (i8 rank = Rank::RANK_8; rank >= Rank::RANK_1; rank--)
            {
                for (u8 file = File::FILE_A; file <= File::FILE_H; file++)
                {
                    const Square sq = RF2SQ(rank, file);
                    std::cout << (char) pieces_str[this->pieces[sq]] << " ";
                }
                std::cout << std::endl;
            }
        }
#endif

    }

}
