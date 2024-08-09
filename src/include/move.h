#ifndef MOVE_H
#define MOVE_H

#include "piece.h"
#include "position.h"
#include <stdbool.h>
#include <stdint.h>

#define MOVE_PIECE(move_id) ((move_id) & 0xF)
#define MOVE_FROM_SQ(move_id) ((move_id >> 4) & 0x7F)
#define MOVE_TO_SQ(move_id) ((move_id >> 11) & 0x7F)
#define MOVE_CAPTURED(move_id) ((move_id >> 18) & 0xF)
#define MOVE_PROMOTED(move_id) ((move_id >> 23) & 0xF)

#define MOVE_FLAG_PS 0x4000000
#define MOVE_FLAG_EP 0x8000000
#define MOVE_FLAG_WKCA 0x10000000
#define MOVE_FLAG_WQCA 0x20000000
#define MOVE_FLAG_BKCA 0x40000000
#define MOVE_FLAG_BQCA 0x80000000

typedef struct {
    uint32_t move_id;
    int16_t  evaluation;
} Move;

typedef struct {
    Move     move;
    uint8_t  prev_casteling_rights;
    uint8_t  prev_enpassant_target;
    uint8_t  prev_half_move_clock;
    uint8_t  prev_full_move_number;
    uint64_t prev_hash;
} MoveHistoryEntry;

Move encode_move(const Piece    piece,
                 const uint8_t  from_sq,
                 const uint8_t  to_sq,
                 const Piece    captured_piece,
                 const Piece    promoted_piece,
                 const bool     flag_ps,
                 const bool     flag_ep,
                 const uint32_t flag_ca);

Move encode_quite_move(const Piece   piece,
                       const uint8_t from_sq,
                       const uint8_t to_sq);

Move encode_capture_move(const Piece   piece,
                         const uint8_t from_sq,
                         const uint8_t to_sq,
                         const Piece   captured_piece);

Move encode_pawn_start_move(const Piece   pawn,
                            const uint8_t from_sq,
                            const uint8_t to_sq);

Move encode_pawn_promotion_move(const Piece   pawn,
                                const uint8_t from_sq,
                                const uint8_t to_sq,
                                const Piece   captured_piece,
                                const Piece   promoted_piece);

Move encode_pawn_enpassant_move(const Piece   pawn,
                                const uint8_t from_sq,
                                const uint8_t to_sq,
                                const Piece   captured_pawn);

Move encode_king_castle_move(const Piece    piece,
                             const uint8_t  from_sq,
                             const uint8_t  to_sq,
                             const uint32_t flag_ca);

bool do_pawn_move(Position* position, const Move move);
bool do_rook_move(Position* position, const Move move);
bool do_king_move(Position* position, const Move move);
bool do_knight_bishop_queen_move(Position* position, const Move move);

void undo_pawn_move(Position* position, Move move);
void undo_king_move(Position* position, Move move);
void undo_piece_move(Position* position, Move move);

void print_move(const Move move);

#endif
