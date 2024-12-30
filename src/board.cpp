#include "board.h"
#include <iostream>

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

        void Board::set_piece(const Piece piece, const Square square) {
            const BitBoard bit          = 1ULL << square;
            const BitBoard bit_inverted = ~(bit);
            bitboards[piece] |= bit;
            bitboards[Piece::NO_PIECE] &= bit_inverted;
            bitboards[BOARD_PIECE_COLOR_IDX(piece)] |= bit;
            pieces[square] = piece;
        }

        void Board::clear_piece(const Piece piece, const Square square) {
            const BitBoard bit          = 1ULL << square;
            const BitBoard bit_inverted = ~(bit);
            bitboards[piece] &= bit_inverted;
            bitboards[Piece::NO_PIECE] |= bit;
            bitboards[BOARD_PIECE_COLOR_IDX(piece)] &= bit_inverted;
            pieces[square] = Piece::NO_PIECE;
        }

        void Board::set_startpos() {
            reset();

            set_piece(Piece::WHITE_ROOK, Square::A1);
            set_piece(Piece::WHITE_KNIGHT, Square::B1);
            set_piece(Piece::WHITE_BISHOP, Square::C1);
            set_piece(Piece::WHITE_QUEEN, Square::D1);
            set_piece(Piece::WHITE_KING, Square::E1);
            set_piece(Piece::WHITE_BISHOP, Square::F1);
            set_piece(Piece::WHITE_KNIGHT, Square::G1);
            set_piece(Piece::WHITE_ROOK, Square::H1);

            set_piece(Piece::WHITE_PAWN, Square::A2);
            set_piece(Piece::WHITE_PAWN, Square::B2);
            set_piece(Piece::WHITE_PAWN, Square::C2);
            set_piece(Piece::WHITE_PAWN, Square::D2);
            set_piece(Piece::WHITE_PAWN, Square::E2);
            set_piece(Piece::WHITE_PAWN, Square::F2);
            set_piece(Piece::WHITE_PAWN, Square::G2);
            set_piece(Piece::WHITE_PAWN, Square::H2);

            set_piece(Piece::BLACK_ROOK, Square::A8);
            set_piece(Piece::BLACK_KNIGHT, Square::B8);
            set_piece(Piece::BLACK_BISHOP, Square::C8);
            set_piece(Piece::BLACK_QUEEN, Square::D8);
            set_piece(Piece::BLACK_KING, Square::E8);
            set_piece(Piece::BLACK_BISHOP, Square::F8);
            set_piece(Piece::BLACK_KNIGHT, Square::G8);
            set_piece(Piece::BLACK_ROOK, Square::H8);

            set_piece(Piece::BLACK_PAWN, Square::A7);
            set_piece(Piece::BLACK_PAWN, Square::B7);
            set_piece(Piece::BLACK_PAWN, Square::C7);
            set_piece(Piece::BLACK_PAWN, Square::D7);
            set_piece(Piece::BLACK_PAWN, Square::E7);
            set_piece(Piece::BLACK_PAWN, Square::F7);
            set_piece(Piece::BLACK_PAWN, Square::G7);
            set_piece(Piece::BLACK_PAWN, Square::H7);
        }

        void Board::move_piece(const Piece,
                               const Square from,
                               const Square to,
                               const bool   is_capture,
                               const bool   is_promotion,
                               const Piece  promoted) {}

        void Board::undo_move_piece(const Piece,
                                    const Square from,
                                    const Square to,
                                    const bool   is_capture,
                                    const bool   is_promotion,
                                    const Piece  promoted) {}

        Piece Board::get_piece(const Square square) const { return pieces[square]; }

        u8 Board::get_piece_count(const Piece piece) const {
            return __builtin_popcountll(bitboards[piece]);
        }

        bool Board::is_valid() const {
            const bool check = (get_piece_count(Piece::WHITE_KING) == 1)
                            && (get_piece_count(Piece::BLACK_KING) == 1)
                            && (!(bitboards[Piece::WHITE_PAWN] & BOARD_MASK_RANK_1()))
                            && (!(bitboards[Piece::WHITE_PAWN] & BOARD_MASK_RANK_8()))
                            && (!(bitboards[Piece::BLACK_PAWN] & BOARD_MASK_RANK_1()))
                            && (!(bitboards[Piece::BLACK_PAWN] & BOARD_MASK_RANK_8()));
            return check;
        }

#ifdef DEBUG
        void Board::display() const {
            const char* pieces_str = ".PNBRQKXXpnbrqk";
            for (i8 rank = Rank::RANK_8; rank >= Rank::RANK_1; rank--)
            {
                for (u8 file = File::FILE_A; file <= File::FILE_H; file++)
                {
                    const Square sq = RF_TO_SQ(rank, file);
                    std::cout << (char) pieces_str[this->pieces[sq]] << " ";
                }
                std::cout << std::endl;
            }
        }
#endif

    }

}
