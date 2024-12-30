#include "board.h"
#include "utils.h"

#ifdef DEBUG
    #include <cassert>
    #include <iostream>
#endif

namespace tejas {

    namespace board {

        Board::Board() { reset(); }

        Board::~Board() {}

        void Board::reset() {
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

        void Board::setPiece(const Piece piece, const Square square) {
            const BitBoard bit          = 1ULL << square;
            const BitBoard bit_inverted = ~(bit);
            bitboards[piece] |= bit;
            bitboards[Piece::NO_PIECE] &= bit_inverted;
            bitboards[bitboardColorSlot(piece)] |= bit;
            pieces[square] = piece;
        }

        void Board::clearPiece(const Piece piece, const Square square) {
            const BitBoard bit          = 1ULL << square;
            const BitBoard bit_inverted = ~(bit);
            bitboards[piece] &= bit_inverted;
            bitboards[Piece::NO_PIECE] |= bit;
            bitboards[bitboardColorSlot(piece)] &= bit_inverted;
            pieces[square] = Piece::NO_PIECE;
        }

        void Board::setStartpos() {
            reset();

            setPiece(Piece::WHITE_ROOK, Square::A1);
            setPiece(Piece::WHITE_KNIGHT, Square::B1);
            setPiece(Piece::WHITE_BISHOP, Square::C1);
            setPiece(Piece::WHITE_QUEEN, Square::D1);
            setPiece(Piece::WHITE_KING, Square::E1);
            setPiece(Piece::WHITE_BISHOP, Square::F1);
            setPiece(Piece::WHITE_KNIGHT, Square::G1);
            setPiece(Piece::WHITE_ROOK, Square::H1);

            setPiece(Piece::WHITE_PAWN, Square::A2);
            setPiece(Piece::WHITE_PAWN, Square::B2);
            setPiece(Piece::WHITE_PAWN, Square::C2);
            setPiece(Piece::WHITE_PAWN, Square::D2);
            setPiece(Piece::WHITE_PAWN, Square::E2);
            setPiece(Piece::WHITE_PAWN, Square::F2);
            setPiece(Piece::WHITE_PAWN, Square::G2);
            setPiece(Piece::WHITE_PAWN, Square::H2);

            setPiece(Piece::BLACK_ROOK, Square::A8);
            setPiece(Piece::BLACK_KNIGHT, Square::B8);
            setPiece(Piece::BLACK_BISHOP, Square::C8);
            setPiece(Piece::BLACK_QUEEN, Square::D8);
            setPiece(Piece::BLACK_KING, Square::E8);
            setPiece(Piece::BLACK_BISHOP, Square::F8);
            setPiece(Piece::BLACK_KNIGHT, Square::G8);
            setPiece(Piece::BLACK_ROOK, Square::H8);

            setPiece(Piece::BLACK_PAWN, Square::A7);
            setPiece(Piece::BLACK_PAWN, Square::B7);
            setPiece(Piece::BLACK_PAWN, Square::C7);
            setPiece(Piece::BLACK_PAWN, Square::D7);
            setPiece(Piece::BLACK_PAWN, Square::E7);
            setPiece(Piece::BLACK_PAWN, Square::F7);
            setPiece(Piece::BLACK_PAWN, Square::G7);
            setPiece(Piece::BLACK_PAWN, Square::H7);
        }

        void Board::movePiece(const Piece  piece,
                              const Square from,
                              const Square to,
                              const bool   is_capture,
                              const bool   is_promotion,
                              const Piece  promoted) {

            clearPiece(piece, from);

            if (is_capture)
            {
                const Piece captured = getPiece(to);
#ifdef DEBUG
                assert(captured != Piece::NO_PIECE);
                assert(pieceColorOf(piece) != pieceColorOf(captured));
                assert(pieceTypeOf(captured) != PieceType::KING);
#endif
                clearPiece(captured, to);
            }

            if (is_promotion)
            {
#ifdef DEBUG
                assert(pieceTypeOf(piece) == PieceType::PAWN);
                if (pieceColorOf(piece) == Color::WHITE)
                {
                    assert(sq2rank(from) == Rank::RANK_7);
                    assert(sq2rank(to) == Rank::RANK_8);
                }
                else
                {
                    assert(sq2rank(from) == Rank::RANK_2);
                    assert(sq2rank(to) == Rank::RANK_1);
                }
                assert(promoted != Piece::NO_PIECE);
                assert(pieceTypeOf(promoted) != PieceType::PAWN);
                assert(pieceTypeOf(promoted) != PieceType::KING);
                assert(pieceColorOf(piece) == pieceColorOf(promoted));
#endif
                setPiece(promoted, to);
            }
            else
            {
                setPiece(piece, to);
            }
        }

        void Board::undoMovePiece(const Piece  piece,
                                  const Square from,
                                  const Square to,
                                  const bool   is_capture,
                                  const Piece  captured,
                                  const bool   is_promotion,
                                  const Piece  promoted) {

            if (is_promotion)
            {
#ifdef DEBUG
                assert(pieceTypeOf(piece) == PieceType::PAWN);
                if (pieceColorOf(piece) == Color::WHITE)
                {
                    assert(sq2rank(from) == Rank::RANK_7);
                    assert(sq2rank(to) == Rank::RANK_8);
                }
                else
                {
                    assert(sq2rank(from) == Rank::RANK_2);
                    assert(sq2rank(to) == Rank::RANK_1);
                }
                assert(promoted != Piece::NO_PIECE);
                assert(pieceTypeOf(promoted) != PieceType::PAWN);
                assert(pieceTypeOf(promoted) != PieceType::KING);
                assert(pieceColorOf(piece) == pieceColorOf(promoted));
#endif
                clearPiece(promoted, to);
            }
            else
            {
                clearPiece(piece, to);
            }

            if (is_capture)
            {
#ifdef DEBUG
                assert(captured != Piece::NO_PIECE);
                assert(pieceColorOf(piece) != pieceColorOf(captured));
                assert(pieceTypeOf(captured) != PieceType::KING);
#endif
                setPiece(captured, to);
            }

            setPiece(piece, from);
        }

        BitBoard Board::getBitboard(const u8 index) const { return bitboards[index]; }

        Piece Board::getPiece(const Square square) const { return pieces[square]; }

        u8 Board::getPieceCount(const Piece piece) const {
            return utils::bitCount1s(bitboards[piece]);
        }

        bool Board::isValid() const {
            const bool check = (getPieceCount(Piece::WHITE_KING) == 1)
                            && (getPieceCount(Piece::BLACK_KING) == 1)
                            && (!(bitboards[Piece::WHITE_PAWN] & MASK_RANK_1))
                            && (!(bitboards[Piece::WHITE_PAWN] & MASK_RANK_8))
                            && (!(bitboards[Piece::BLACK_PAWN] & MASK_RANK_1))
                            && (!(bitboards[Piece::BLACK_PAWN] & MASK_RANK_8));
            return check;
        }

#ifdef DEBUG
        void Board::display() const {
            const char* pieces_str = ".PNBRQKXXpnbrqk";
            for (i8 rank = Rank::RANK_8; rank >= Rank::RANK_1; rank--)
            {
                for (u8 file = File::FILE_A; file <= File::FILE_H; file++)
                {
                    const Square sq = rf2sq(rank, file);
                    std::cout << (char) pieces_str[this->pieces[sq]] << " ";
                }
                std::cout << std::endl;
            }
        }
#endif

    }

}
