#include "board.h"
#include "utils.h"
#include <iostream>
#include <sstream>
#ifdef DEBUG
    #include <cassert>
#endif

namespace tejas {

    namespace board {

        static u64 ZOBRIST_TABLE[15][64];
        static u64 ZOBRIST_WKCA;
        static u64 ZOBRIST_WQCA;
        static u64 ZOBRIST_BKCA;
        static u64 ZOBRIST_BQCA;
        static u64 ZOBRIST_BLACK_TO_MOVE;

        void Board::initialize() {
            for (u8 p = Piece::NO_PIECE; p <= Piece::BLACK_KING; p++)
            {
                for (u8 sq = Square::A1; sq <= Square::H8; sq++)
                {
                    ZOBRIST_TABLE[p][sq] = utils::prng();
                }
            }

            ZOBRIST_WKCA          = utils::prng();
            ZOBRIST_WQCA          = utils::prng();
            ZOBRIST_BKCA          = utils::prng();
            ZOBRIST_BQCA          = utils::prng();
            ZOBRIST_BLACK_TO_MOVE = utils::prng();
        }

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

            active_color     = Color::WHITE;
            casteling_rights = CastleFlag::NOCA;
            enpassant_target = Square::NO_SQ;
            half_move_clock  = 0;
            full_move_number = 0;
            ply_count        = 0;
            hash             = 0ULL;
        }

        void Board::resetHash() {
            hash = 0ULL;

            if (active_color == Color::BLACK)
            {
                hash ^= ZOBRIST_BLACK_TO_MOVE;
            }

            if (casteling_rights & CastleFlag::WKCA)
            {
                hash ^= ZOBRIST_WKCA;
            }

            if (casteling_rights & CastleFlag::WQCA)
            {
                hash ^= ZOBRIST_WQCA;
            }

            if (casteling_rights & CastleFlag::BKCA)
            {
                hash ^= ZOBRIST_BKCA;
            }

            if (casteling_rights & CastleFlag::BQCA)
            {
                hash ^= ZOBRIST_BQCA;
            }

            for (u8 sq = Square::A1; sq <= Square::H8; sq++)
            {
                const Piece p = getPiece(static_cast<Square>(sq));

                if (p == Piece::NO_PIECE)
                {
                    continue;
                }

                hash ^= ZOBRIST_TABLE[p][sq];
            }
        }

        void Board::setPiece(const Piece piece, const Square square) {
#ifdef DEBUG
            assert(piece != Piece::NO_PIECE);
            assert(square != Square::NO_SQ);
#endif
            const BitBoard bit          = 1ULL << square;
            const BitBoard bit_inverted = ~(bit);
            bitboards[piece] |= bit;
            bitboards[Piece::NO_PIECE] &= bit_inverted;
            bitboards[bitboardColorSlot(piece)] |= bit;
            pieces[square] = piece;
            hash ^= ZOBRIST_TABLE[piece][square];
        }

        void Board::clearPiece(const Piece piece, const Square square) {
#ifdef DEBUG
            assert(piece != Piece::NO_PIECE);
            assert(square != Square::NO_SQ);
#endif
            const BitBoard bit          = 1ULL << square;
            const BitBoard bit_inverted = ~(bit);
            bitboards[piece] &= bit_inverted;
            bitboards[Piece::NO_PIECE] |= bit;
            bitboards[bitboardColorSlot(piece)] &= bit_inverted;
            pieces[square] = Piece::NO_PIECE;
            hash ^= ZOBRIST_TABLE[piece][square];
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

            active_color = Color::WHITE;
            casteling_rights =
              CastleFlag::WKCA | CastleFlag::WQCA | CastleFlag::BKCA | CastleFlag::BQCA;
            enpassant_target = Square::NO_SQ;
            half_move_clock  = 0;
            full_move_number = 1;
            ply_count        = 0;

            resetHash();
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
                assert(pieceColorOf(piece) == colorFlip(pieceColorOf(captured)));
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
                assert(pieceColorOf(piece) == colorFlip(pieceColorOf(captured)));
                assert(pieceTypeOf(captured) != PieceType::KING);
#endif
                setPiece(captured, to);
            }

            setPiece(piece, from);
        }

        void Board::setActiveColor(const Color c) { active_color = c; }

        void Board::addCastelingRights(const CastleFlag f) { casteling_rights |= f; }

        void Board::setEnpassantTarget(const Square s) { enpassant_target = s; }

        void Board::setHalfmoveClock(const u8 n) { half_move_clock = n; }

        void Board::setFullmoveNumber(const u8 n) { full_move_number = n; }

        void Board::resetPlyCount() { ply_count = 0; }

        BitBoard Board::getBitboard(const u8 index) const { return bitboards[index]; }

        Piece Board::getPiece(const Square square) const { return pieces[square]; }

        u8 Board::getPieceCount(const Piece piece) const {
            return utils::bitCount1s(bitboards[piece]);
        }

        Color Board::getActiveColor() const { return active_color; }

        u8 Board::getCastelingRights() const { return casteling_rights; }

        Square Board::getEnpassantTarget() const { return enpassant_target; }

        u8 Board::getHalfmoveClock() const { return half_move_clock; }

        u8 Board::getFullmoveNumber() const { return full_move_number; }

        i8 Board::getPlyCount() const { return ply_count; }

        u64 Board::getHash() const { return hash; }

        bool Board::isValid() const {
            const bool check = (getPieceCount(Piece::WHITE_KING) == 1)
                            && (getPieceCount(Piece::BLACK_KING) == 1)
                            && (!(bitboards[Piece::WHITE_PAWN] & MASK_RANK_1))
                            && (!(bitboards[Piece::WHITE_PAWN] & MASK_RANK_8))
                            && (!(bitboards[Piece::BLACK_PAWN] & MASK_RANK_1))
                            && (!(bitboards[Piece::BLACK_PAWN] & MASK_RANK_8));
            return check;
        }

        bool Board::operator==(Board const& rhs) const { return hash == rhs.getHash(); }

        void Board::display() const {
            const char* pieces_str = ".PNBRQKXXpnbrqk";
            const char* color_str  = "wb";

            std::stringstream ss;

            for (i8 rank = Rank::RANK_8; rank >= Rank::RANK_1; rank--)
            {
                for (u8 file = File::FILE_A; file <= File::FILE_H; file++)
                {
                    const Square sq = rf2sq(rank, file);
                    ss << (char) pieces_str[this->pieces[sq]] << " ";
                }
                ss << "\n";
            }

            ss << "\n" << (char) color_str[active_color];
            ss << " " << (int) casteling_rights;
            ss << " " << (int) enpassant_target;
            ss << " " << (int) half_move_clock;
            ss << " " << (int) full_move_number;
            ss << " " << (int) ply_count;
            ss << " " << (unsigned long long) hash << "\n";

            std::cout << ss.str() << std::endl;
        }

    }

}
