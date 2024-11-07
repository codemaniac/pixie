#include "include/position.h"
#include "include/board.h"
#include "include/constants.h"
#include "include/move.h"
#include "include/utils.h"
#include <cassert>
#include <cinttypes>
#include <cstdint>
#include <iostream>

constexpr uint8_t HASH_IDX(Piece p) { return ((p > 8) ? p - 3 : p - 1); }
static uint64_t   HASHTABLE[12][64];
static uint64_t   HASH_WKCA;
static uint64_t   HASH_WQCA;
static uint64_t   HASH_BKCA;
static uint64_t   HASH_BQCA;
static uint64_t   HASH_BLACK_TO_MOVE;

// Little-Endian Rank-File Mapping
// clang-format off
static const uint8_t CASTLE_RIGHTS_MODIFIERS[64] = {
    13, 15, 15, 15, 12, 15, 15, 14,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
     7, 15, 15, 15,  3, 15, 15, 11
};
// clang-format on

void position_init() {
    board_init();
    for (int p = 0; p < 12; p++)
    {
        for (int sq = 0; sq < 64; sq++)
            HASHTABLE[p][sq] = utils_random_uint64();
    }
    HASH_WKCA          = utils_random_uint64();
    HASH_WQCA          = utils_random_uint64();
    HASH_BKCA          = utils_random_uint64();
    HASH_BQCA          = utils_random_uint64();
    HASH_BLACK_TO_MOVE = utils_random_uint64();
}

Position::Position() {
    this->board            = Board();
    this->active_color     = WHITE;
    this->casteling_rights = NOCA;
    this->enpassant_target = NO_SQ;
    this->half_move_clock  = 0;
    this->full_move_number = 0;
    this->ply_count        = 0;
    this->hash             = 0ULL;
}

Position::Position(const Position& position) {
    this->board            = position.board;  // TODO: Optimize
    this->active_color     = position.active_color;
    this->casteling_rights = position.casteling_rights;
    this->enpassant_target = position.enpassant_target;
    this->half_move_clock  = position.half_move_clock;
    this->full_move_number = position.full_move_number;
    this->ply_count        = position.ply_count;
    this->hash             = position.hash;
}

void Position::set_piece(const Piece piece, const Square square) {
    this->board.set_piece(piece, square);
    this->hash ^= HASHTABLE[HASH_IDX(piece)][square];
}
void Position::clear_piece(const Piece piece, const Square square) {
    this->board.clear_piece(piece, square);
    this->hash ^= HASHTABLE[HASH_IDX(piece)][square];
}

void Position::set_active_color(const Color active_color) { this->active_color = active_color; }

void Position::add_casteling_rights(const CastleFlag castle_flag) {
    this->casteling_rights |= castle_flag;
}
void Position::set_enpassant_target(const Square enpassant_target) {
    this->enpassant_target = enpassant_target;
}
void Position::set_half_move_clock(const uint32_t half_move_clock) {
    this->half_move_clock = half_move_clock;
}
void Position::set_full_move_number(const uint32_t full_move_number) {
    this->full_move_number = full_move_number;
}

void Position::reset_ply_count() { this->ply_count = 0; }

void Position::reset_hash() {
    this->hash = 0ULL;

    if (this->active_color == BLACK)
        this->hash ^= HASH_BLACK_TO_MOVE;

    if (this->casteling_rights & WKCA)
        this->hash ^= HASH_WKCA;

    if (this->casteling_rights & WQCA)
        this->hash ^= HASH_WQCA;

    if (this->casteling_rights & BKCA)
        this->hash ^= HASH_BKCA;

    if (this->casteling_rights & BQCA)
        this->hash ^= HASH_BQCA;

    for (uint8_t sq = 0; sq < 64; sq++)
    {
        const Piece p = this->board.get_piece(static_cast<Square>(sq));
        if (p == NO_PIECE)
            continue;

        const uint8_t piece_hash_idx = HASH_IDX(p);
        this->hash ^= HASHTABLE[piece_hash_idx][sq];
    }
}

void Position::set_start_pos() {
    this->board.set_start_pos();
    this->active_color     = WHITE;
    this->casteling_rights = WKCA | WQCA | BKCA | BQCA;
    this->enpassant_target = NO_SQ;
    this->half_move_clock  = 0;
    this->full_move_number = 1;
    this->ply_count        = 0;
    this->reset_hash();
}

bool Position::move_do_on_complete() {
    const bool is_legal_move = (!this->is_in_check()) && this->is_valid();
    this->active_color       = static_cast<Color>(this->active_color ^ 1);
    if (this->active_color == BLACK)
        this->hash ^= HASH_BLACK_TO_MOVE;
    return is_legal_move;
}

bool Position::move_do(const Move move) {
    const Square   move_from_sq = move.get_from();
    const Square   move_to_sq   = move.get_to();
    const Piece    move_piece   = this->get_piece(move_from_sq);
    const MoveFlag move_flag    = move.get_flag();

    if (PIECE_GET_COLOR(move_piece) != this->active_color)
        return false;

    const MoveHistoryEntry mhe(move, this->casteling_rights, this->enpassant_target,
                               this->half_move_clock, this->full_move_number, this->hash);
    this->history.push(mhe);

    this->enpassant_target = NO_SQ;
    this->half_move_clock++;
    if (this->active_color == BLACK)
        this->full_move_number++;
    this->ply_count++;

    this->clear_piece(move_piece, move_from_sq);

    if (move_flag == MOVE_QUIET_PAWN_DBL_PUSH)
    {
        const int8_t side_to_move = 1 - (2 * this->active_color);  // WHITE = 1; BLACK = -1
        this->enpassant_target    = static_cast<Square>(move_from_sq + (8 * side_to_move));
        this->set_piece(move_piece, move_to_sq);
        this->half_move_clock = 0;
        return this->move_do_on_complete();
    }

    if (move_flag == MOVE_CAPTURE_EP)
    {
        const int8_t side_to_move        = -1 + (2 * this->active_color);  // WHITE = -1; BLACK = 1
        const Square ep_captured_pawn_sq = static_cast<Square>(move_to_sq + (8 * side_to_move));
        const Piece  ep_captured_pawn =
          PIECE_CREATE(PAWN, static_cast<Color>(this->active_color ^ 1));
        this->clear_piece(ep_captured_pawn, ep_captured_pawn_sq);
        this->set_piece(move_piece, move_to_sq);
        this->half_move_clock = 0;
        return this->move_do_on_complete();
    }

    if (move_flag == MOVE_CASTLE_KING_SIDE)
    {
        const Piece king = PIECE_CREATE(KING, this->active_color);
        const Piece rook = PIECE_CREATE(ROOK, this->active_color);
        this->set_piece(king, move_to_sq);
        this->clear_piece(rook, static_cast<Square>(move_to_sq + 1));
        this->set_piece(rook, static_cast<Square>(move_to_sq - 1));
        this->casteling_rights &= CASTLE_RIGHTS_MODIFIERS[move_from_sq];
        this->hash ^= this->active_color == WHITE ? HASH_WKCA : HASH_BKCA;
        return this->move_do_on_complete();
    }

    if (move_flag == MOVE_CASTLE_QUEEN_SIDE)
    {
        const Piece king = PIECE_CREATE(KING, this->active_color);
        const Piece rook = PIECE_CREATE(ROOK, this->active_color);
        this->set_piece(king, move_to_sq);
        this->clear_piece(rook, static_cast<Square>(move_to_sq - 2));
        this->set_piece(rook, static_cast<Square>(move_to_sq + 1));
        this->casteling_rights &= CASTLE_RIGHTS_MODIFIERS[move_from_sq];
        this->hash ^= this->active_color == WHITE ? HASH_WQCA : HASH_BQCA;
        return this->move_do_on_complete();
    }

    if (MOVE_IS_CAPTURE(move_flag))
    {
        this->clear_piece(this->board.get_piece(move_to_sq), move_to_sq);
        this->half_move_clock = 0;
    }

    if (MOVE_IS_PROMOTION(move_flag))
    {
        switch (move_flag)
        {
            case MOVE_PROMOTION_KNIGHT :
            case MOVE_CAPTURE_PROMOTION_KNIGHT :
                this->set_piece(PIECE_CREATE(KNIGHT, this->active_color), move_to_sq);
                break;
            case MOVE_PROMOTION_BISHOP :
            case MOVE_CAPTURE_PROMOTION_BISHOP :
                this->set_piece(PIECE_CREATE(BISHOP, this->active_color), move_to_sq);
                break;
            case MOVE_PROMOTION_ROOK :
            case MOVE_CAPTURE_PROMOTION_ROOK :
                this->set_piece(PIECE_CREATE(ROOK, this->active_color), move_to_sq);
                break;
            case MOVE_PROMOTION_QUEEN :
            case MOVE_CAPTURE_PROMOTION_QUEEN :
                this->set_piece(PIECE_CREATE(QUEEN, this->active_color), move_to_sq);
                break;
            default :
                break;
        }
    }
    else
    {
        this->set_piece(move_piece, move_to_sq);
    }

    if (PIECE_GET_TYPE(move_piece) == PAWN)
        this->half_move_clock = 0;

    this->casteling_rights &=
      (CASTLE_RIGHTS_MODIFIERS[move_from_sq] & CASTLE_RIGHTS_MODIFIERS[move_to_sq]);

    return this->move_do_on_complete();
}

bool Position::move_do(const std::string move_str) {
    if (move_str.length() < 4 || move_str.length() > 5)
        throw std::invalid_argument("Invalid move string!");

    const File   from_file = static_cast<File>(move_str[0] - 'a');
    const Rank   from_rank = static_cast<Rank>((move_str[1] - '0') - 1);
    const File   to_file   = static_cast<File>(move_str[2] - 'a');
    const Rank   to_rank   = static_cast<Rank>((move_str[3] - '0') - 1);
    const Square from_sq   = BOARD_RF_TO_SQ(from_rank, from_file);
    const Square to_sq     = BOARD_RF_TO_SQ(to_rank, to_file);

    const Piece move_piece     = this->get_piece(from_sq);
    Piece       captured_piece = this->get_piece(to_sq);

    const Rank pawn_promotion_rank = static_cast<Rank>(7 * (this->active_color ^ 1));
    const bool is_promotion_move   = (move_str.length() == 5) && (to_rank == pawn_promotion_rank);

    MoveFlag flag;

    if (captured_piece == NO_PIECE)
    {
        if (PIECE_GET_TYPE(move_piece) == PAWN)
        {
            if (this->active_color == WHITE && from_rank == RANK_2 && to_rank == RANK_4)
                flag = MOVE_QUIET_PAWN_DBL_PUSH;
            else if (this->active_color == BLACK && from_rank == RANK_7 && to_rank == RANK_5)
                flag = MOVE_QUIET_PAWN_DBL_PUSH;
            else if (to_sq == this->enpassant_target)
            {
                captured_piece = PIECE_CREATE(PAWN, static_cast<Color>(this->active_color ^ 1));
                flag           = MOVE_CAPTURE_EP;
            }
            else if (is_promotion_move)
            {
                switch (move_str[4])
                {
                    case 'n' :
                        flag = MOVE_PROMOTION_KNIGHT;
                        break;
                    case 'b' :
                        flag = MOVE_PROMOTION_BISHOP;
                        break;
                    case 'r' :
                        flag = MOVE_PROMOTION_ROOK;
                        break;
                    case 'q' :
                        flag = MOVE_PROMOTION_QUEEN;
                        break;
                    default :
                        flag = MOVE_PROMOTION_QUEEN;
                        break;
                }
            }
            else
                flag = MOVE_QUIET;
        }
        else if (PIECE_GET_TYPE(move_piece) == KING)
        {
            if (this->active_color == WHITE)
            {
                if ((this->casteling_rights & WKCA) && from_sq == E1 && to_sq == G1)
                    flag = MOVE_CASTLE_KING_SIDE;
                else if ((this->casteling_rights & WQCA) && from_sq == E1 && to_sq == C1)
                    flag = MOVE_CASTLE_QUEEN_SIDE;
                else
                    flag = MOVE_QUIET;
            }
            else
            {
                if ((this->casteling_rights & BKCA) && from_sq == E8 && to_sq == G8)
                    flag = MOVE_CASTLE_KING_SIDE;
                else if ((this->casteling_rights & BQCA) && from_sq == E8 && to_sq == C8)
                    flag = MOVE_CASTLE_QUEEN_SIDE;
                else
                    flag = MOVE_QUIET;
            }
        }
        else
            flag = MOVE_QUIET;
    }
    else
    {
        if (PIECE_GET_TYPE(move_piece) == PAWN)
        {
            if (is_promotion_move)
            {
                switch (move_str[4])
                {
                    case 'n' :
                        flag = MOVE_CAPTURE_PROMOTION_KNIGHT;
                        break;
                    case 'b' :
                        flag = MOVE_CAPTURE_PROMOTION_BISHOP;
                        break;
                    case 'r' :
                        flag = MOVE_CAPTURE_PROMOTION_ROOK;
                        break;
                    case 'q' :
                        flag = MOVE_CAPTURE_PROMOTION_QUEEN;
                        break;
                    default :
                        flag = MOVE_CAPTURE_PROMOTION_QUEEN;
                        break;
                }
            }
            else
                flag = MOVE_CAPTURE;
        }
        else
            flag = MOVE_CAPTURE;
    }

    const uint16_t move_score = captured_piece == NO_PIECE
                                ? 0
                                : MOVE_SCORE_MVV_LVA[MOVE_SCORE_MVV_LVA_IDX(
                                    PIECE_GET_TYPE(move_piece), PIECE_GET_TYPE(captured_piece))];

    const Move move(from_sq, to_sq, flag, captured_piece, move_score);

    return this->move_do(move);
}

void Position::move_do_null() {
    const Move             null_move;
    const MoveHistoryEntry mhe(null_move, this->casteling_rights, this->enpassant_target,
                               this->half_move_clock, this->full_move_number, this->hash);
    this->history.push(mhe);
    this->enpassant_target = NO_SQ;
    this->active_color     = static_cast<Color>(this->active_color ^ 1);
    if (this->active_color == BLACK)
        this->hash ^= HASH_BLACK_TO_MOVE;
}

void Position::move_undo() {
    const MoveHistoryEntry& mhe               = this->history.top();
    const Move              prev_move         = mhe.move;
    const Square            prev_move_from_sq = prev_move.get_from();
    const Square            prev_move_to_sq   = prev_move.get_to();
    const MoveFlag          prev_move_flag    = prev_move.get_flag();
    const Color             prev_active_color = static_cast<Color>(this->active_color ^ 1);

    if (prev_move_flag == MOVE_CASTLE_KING_SIDE)
    {
        const Piece king = PIECE_CREATE(KING, prev_active_color);
        const Piece rook = PIECE_CREATE(ROOK, prev_active_color);
        this->clear_piece(king, prev_move_to_sq);
        this->set_piece(king, prev_move_from_sq);
        this->clear_piece(rook, static_cast<Square>(prev_move_to_sq - 1));
        this->set_piece(rook, static_cast<Square>(prev_move_to_sq + 1));
        this->hash ^= this->active_color == BLACK ? HASH_WKCA : HASH_BKCA;
    }
    else if (prev_move_flag == MOVE_CASTLE_QUEEN_SIDE)
    {
        const Piece king = PIECE_CREATE(KING, prev_active_color);
        const Piece rook = PIECE_CREATE(ROOK, prev_active_color);
        this->clear_piece(king, prev_move_to_sq);
        this->set_piece(king, prev_move_from_sq);
        this->clear_piece(rook, static_cast<Square>(prev_move_to_sq + 1));
        this->set_piece(rook, static_cast<Square>(prev_move_to_sq - 2));
        this->hash ^= this->active_color == BLACK ? HASH_WQCA : HASH_BQCA;
    }
    else if (prev_move_flag == MOVE_CAPTURE_EP)
    {
        const Piece pawn = PIECE_CREATE(PAWN, prev_active_color);
        this->clear_piece(pawn, prev_move_to_sq);
        this->set_piece(pawn, prev_move_from_sq);

        const Piece  ep_captured_pawn = PIECE_CREATE(PAWN, this->active_color);
        const int8_t side_to_move     = 1 - (2 * this->active_color);  // WHITE = 1; BLACK = -1
        const Square ep_captured_pawn_sq =
          static_cast<Square>(prev_move_to_sq + (8 * side_to_move));
        this->set_piece(ep_captured_pawn, ep_captured_pawn_sq);
    }
    else if (MOVE_IS_PROMOTION(prev_move_flag))
    {
        const Piece promoted_piece = this->board.get_piece(prev_move_to_sq);
        this->clear_piece(promoted_piece, prev_move_to_sq);
        this->set_piece(PIECE_CREATE(PAWN, prev_active_color), prev_move_from_sq);
        if (MOVE_IS_CAPTURE(prev_move_flag))
            this->set_piece(prev_move.get_captured(), prev_move_to_sq);
    }
    else if (MOVE_IS_CAPTURE(prev_move_flag))
    {
        Piece prev_move_piece = this->board.get_piece(prev_move_to_sq);
        this->clear_piece(prev_move_piece, prev_move_to_sq);
        this->set_piece(prev_move.get_captured(), prev_move_to_sq);
        this->set_piece(prev_move_piece, prev_move_from_sq);
    }
    else
    {
        Piece prev_move_piece = this->board.get_piece(prev_move_to_sq);
        this->clear_piece(prev_move_piece, prev_move_to_sq);
        this->set_piece(prev_move_piece, prev_move_from_sq);
    }

    if (this->active_color == BLACK)
        this->hash ^= HASH_BLACK_TO_MOVE;

    assert(this->hash == mhe.prev_hash);
    assert((this->active_color ^ 1) == prev_active_color);

    this->active_color     = prev_active_color;
    this->casteling_rights = mhe.prev_casteling_rights;
    this->enpassant_target = mhe.prev_enpassant_target;
    this->half_move_clock  = mhe.prev_half_move_clock;
    this->full_move_number = mhe.prev_full_move_number;
    this->ply_count--;

    this->history.pop();
}

void Position::move_undo_null() {
    const MoveHistoryEntry& mhe               = this->history.top();
    const Color             prev_active_color = static_cast<Color>(this->active_color ^ 1);

    if (this->active_color == BLACK)
        this->hash ^= HASH_BLACK_TO_MOVE;

    assert(this->hash == mhe.prev_hash);
    assert((this->active_color ^ 1) == prev_active_color);

    this->active_color     = prev_active_color;
    this->casteling_rights = mhe.prev_casteling_rights;
    this->enpassant_target = mhe.prev_enpassant_target;
    this->half_move_clock  = mhe.prev_half_move_clock;
    this->full_move_number = mhe.prev_full_move_number;

    this->history.pop();
}

Piece Position::get_piece(const Square square) const { return this->board.get_piece(square); }

uint8_t Position::get_piece_count(const Piece piece) const {
    return this->board.get_piece_count(piece);
}

Color Position::get_active_color() const { return this->active_color; }

uint32_t Position::get_half_move_clock() const { return this->half_move_clock; }

uint32_t Position::get_ply_count() const { return this->ply_count; }

uint64_t Position::get_hash() const { return this->hash; }

bool Position::is_valid() const { return this->board.is_valid(); }

bool Position::is_in_check() const { return this->board.is_in_check(this->active_color); }

bool Position::is_repeated() const {
    for (uint16_t i = (this->ply_count - this->half_move_clock); i < this->ply_count - 1; ++i)
    {
        if (this->hash == this->history.peek(i).prev_hash)
            return true;
    }
    return false;
}

void Position::generate_pseudolegal_moves(ArrayList<Move>* move_list,
                                          const bool       only_captures) const {
    this->board.generate_pseudolegal_moves(this->active_color, this->casteling_rights,
                                           this->enpassant_target, only_captures, move_list);
}

void Position::display() const {
    this->board.display();
    const std::string color_str = "wb";
    printf("\n%c %d %d %d %d %d\n%" PRIu64 "\n\n", color_str[this->active_color],
           this->casteling_rights, this->enpassant_target, this->half_move_clock,
           this->full_move_number, this->ply_count, this->hash);
}
