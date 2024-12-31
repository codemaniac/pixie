#include "board.h"
#include "movegen.h"
#include "utils.h"
#include <iostream>
#include <sstream>
#ifdef DEBUG
    #include <cassert>
#endif

namespace tejas {

    namespace board {

        // Little-Endian Rank-File Mapping
        // clang-format off
        static const u8 CASTLE_RIGHTS_MODIFIERS[64] = {
            13, 15, 15, 15, 12, 15, 15, 14,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            7,  15, 15, 15,  3, 15, 15, 11
        };
        // clang-format on

        static u64 ZOBRIST_TABLE[15][64];
        static u64 ZOBRIST_KCA[2];
        static u64 ZOBRIST_QCA[2];
        static u64 ZOBRIST_BLACK_TO_MOVE;

        void Board::initialize() {
            for (u8 p = Piece::NO_PIECE; p <= Piece::BLACK_KING; p++)
            {
                for (u8 sq = Square::A1; sq <= Square::H8; sq++)
                {
                    ZOBRIST_TABLE[p][sq] = utils::prng();
                }
            }

            ZOBRIST_KCA[Color::WHITE] = utils::prng();
            ZOBRIST_KCA[Color::BLACK] = utils::prng();
            ZOBRIST_QCA[Color::WHITE] = utils::prng();
            ZOBRIST_QCA[Color::BLACK] = utils::prng();
            ZOBRIST_BLACK_TO_MOVE     = utils::prng();
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
                hash ^= ZOBRIST_KCA[Color::WHITE];
            }

            if (casteling_rights & CastleFlag::WQCA)
            {
                hash ^= ZOBRIST_QCA[Color::WHITE];
            }

            if (casteling_rights & CastleFlag::BKCA)
            {
                hash ^= ZOBRIST_KCA[Color::BLACK];
            }

            if (casteling_rights & CastleFlag::BQCA)
            {
                hash ^= ZOBRIST_QCA[Color::BLACK];
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
                assert(sq2rank(from) == promotionRankSrcOf(pieceColorOf(piece)));
                assert(sq2rank(to) == promotionRankDestOf(pieceColorOf(piece)));
                assert(promoted != Piece::NO_PIECE);
                assert(pieceTypeOf(promoted) != PieceType::PAWN);
                assert(pieceTypeOf(promoted) != PieceType::KING);
                assert(pieceColorOf(piece) == pieceColorOf(promoted));
                assert(getPiece(to) == Piece::NO_PIECE);
#endif
                setPiece(promoted, to);
            }
            else
            {
#ifdef DEBUG
                assert(getPiece(to) == Piece::NO_PIECE);
#endif
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
                assert(sq2rank(from) == promotionRankSrcOf(pieceColorOf(piece)));
                assert(sq2rank(to) == promotionRankDestOf(pieceColorOf(piece)));
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

#ifdef DEBUG
            assert(getPiece(from) == Piece::NO_PIECE);
#endif
            setPiece(piece, from);
        }

        void Board::setActiveColor(const Color c) { active_color = c; }

        void Board::addCastelingRights(const CastleFlag f) { casteling_rights |= f; }

        void Board::setEnpassantTarget(const Square s) { enpassant_target = s; }

        void Board::setHalfmoveClock(const u8 n) { half_move_clock = n; }

        void Board::setFullmoveNumber(const u8 n) { full_move_number = n; }

        void Board::resetPlyCount() { ply_count = 0; }

        bool Board::doMoveComplete() {
            const bool is_valid_move = !movegen::isInCheck(*this) && isValid();
            active_color             = colorFlip(active_color);
            if (active_color == Color::BLACK)
            {
                hash ^= ZOBRIST_BLACK_TO_MOVE;
            }
            return is_valid_move;
        }

        [[nodiscard]] bool Board::doMove(const move::Move move) {
            const Piece          piece = move.getPiece();
            const Square         from  = move.getFrom();
            const Square         to    = move.getTo();
            const move::MoveFlag flag  = move.getFlag();

            if (pieceColorOf(piece) == colorFlip(active_color))
            {
                return false;
            }

            history.emplace_back(move, casteling_rights, enpassant_target, half_move_clock,
                                 full_move_number, hash);

            enpassant_target = Square::NO_SQ;
            half_move_clock++;
            if (active_color == Color::BLACK)
            {
                full_move_number++;
            }
            ply_count++;

            if (flag == move::MoveFlag::MOVE_QUIET_PAWN_DBL_PUSH)
            {
                movePiece(piece, from, to);
                const i8 stm     = 1 - (2 * active_color);  // WHITE = 1; BLACK = -1
                enpassant_target = static_cast<Square>(from + (8 * stm));
                half_move_clock  = 0;
                return doMoveComplete();
            }
            else if (flag == move::MoveFlag::MOVE_CAPTURE_EP)
            {
                movePiece(piece, from, to);
                const i8     stm         = -1 + (2 * active_color);  // WHITE = -1; BLACK = 1
                const Square captured_sq = static_cast<Square>(to + (8 * stm));
                const Piece  captured    = pieceCreate(PieceType::PAWN, colorFlip(active_color));
                clearPiece(captured, captured_sq);
                half_move_clock = 0;
                return doMoveComplete();
            }
            else if (flag == move::MoveFlag::MOVE_CASTLE_KING_SIDE)
            {
                movePiece(piece, from, to);
                const Piece rook = pieceCreate(PieceType::ROOK, active_color);
                movePiece(rook, static_cast<Square>(to + 1), static_cast<Square>(to - 1));
                casteling_rights &= CASTLE_RIGHTS_MODIFIERS[from];
                hash ^= ZOBRIST_KCA[active_color];
                return doMoveComplete();
            }
            else if (flag == move::MoveFlag::MOVE_CASTLE_QUEEN_SIDE)
            {
                movePiece(piece, from, to);
                const Piece rook = pieceCreate(PieceType::ROOK, active_color);
                movePiece(rook, static_cast<Square>(to - 2), static_cast<Square>(to + 1));
                casteling_rights &= CASTLE_RIGHTS_MODIFIERS[from];
                hash ^= ZOBRIST_QCA[active_color];
                return doMoveComplete();
            }

            Piece promoted;

            switch (flag)
            {
                case move::MoveFlag::MOVE_PROMOTION_KNIGHT :
                case move::MoveFlag::MOVE_CAPTURE_PROMOTION_KNIGHT :
                    promoted = pieceCreate(PieceType::KNIGHT, active_color);
                    break;
                case move::MoveFlag::MOVE_PROMOTION_BISHOP :
                case move::MoveFlag::MOVE_CAPTURE_PROMOTION_BISHOP :
                    promoted = pieceCreate(PieceType::BISHOP, active_color);
                    break;
                case move::MoveFlag::MOVE_PROMOTION_ROOK :
                case move::MoveFlag::MOVE_CAPTURE_PROMOTION_ROOK :
                    promoted = pieceCreate(PieceType::ROOK, active_color);
                    break;
                case move::MoveFlag::MOVE_PROMOTION_QUEEN :
                case move::MoveFlag::MOVE_CAPTURE_PROMOTION_QUEEN :
                    promoted = pieceCreate(PieceType::QUEEN, active_color);
                    break;
                default :
                    promoted = Piece::NO_PIECE;
            }

            movePiece(piece, from, to, move::isCapture(flag), move::isPromotion(flag), promoted);

            if (pieceTypeOf(piece) == PieceType::PAWN)
            {
                half_move_clock = 0;
            }
            casteling_rights &= (CASTLE_RIGHTS_MODIFIERS[from] & CASTLE_RIGHTS_MODIFIERS[to]);

            return doMoveComplete();
        }

        void Board::undoMove() {
            const MoveHistoryEntry& history_entry = history.back();

            const move::Move     move              = history_entry.move;
            const Piece          piece             = move.getPiece();
            const Square         from              = move.getFrom();
            const Square         to                = move.getTo();
            const move::MoveFlag flag              = move.getFlag();
            const Color          prev_active_color = colorFlip(active_color);

            if (flag == move::MoveFlag::MOVE_CASTLE_KING_SIDE)
            {
                undoMovePiece(piece, from, to);
                const Piece rook = pieceCreate(PieceType::ROOK, prev_active_color);
                undoMovePiece(rook, static_cast<Square>(to + 1), static_cast<Square>(to - 1));
                hash ^= ZOBRIST_KCA[prev_active_color];
            }
            else if (flag == move::MoveFlag::MOVE_CASTLE_QUEEN_SIDE)
            {
                undoMovePiece(piece, from, to);
                const Piece rook = pieceCreate(PieceType::ROOK, prev_active_color);
                undoMovePiece(rook, static_cast<Square>(to - 2), static_cast<Square>(to + 1));
                hash ^= ZOBRIST_QCA[prev_active_color];
            }
            else if (flag == move::MoveFlag::MOVE_CAPTURE_EP)
            {
                undoMovePiece(piece, from, to);
                const i8     stm         = 1 - (2 * active_color);  // WHITE = 1; BLACK = -1
                const Square captured_sq = static_cast<Square>(to + (8 * stm));
                const Piece  captured    = pieceCreate(PieceType::PAWN, active_color);
                setPiece(captured, captured_sq);
            }
            else
            {
                const Piece promoted = move::isPromotion(flag) ? getPiece(to) : Piece::NO_PIECE;
                undoMovePiece(piece, from, to, move::isCapture(flag), move.getCaptured(),
                              move::isPromotion(flag), promoted);
            }

            if (active_color == Color::BLACK)
            {
                hash ^= ZOBRIST_BLACK_TO_MOVE;
            }

#ifdef DEBUG
            assert(hash == history_entry.hash);
            assert(colorFlip(active_color) == prev_active_color);
#endif

            active_color     = prev_active_color;
            casteling_rights = history_entry.casteling_rights;
            enpassant_target = history_entry.enpassant_target;
            half_move_clock  = history_entry.half_move_clock;
            full_move_number = history_entry.full_move_number;
            ply_count--;

            history.pop_back();
        }

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
