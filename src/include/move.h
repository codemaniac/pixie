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
  int16_t evaluation;
} Move;

typedef struct {
  Move *move;
  uint8_t prev_casteling_rights;
  uint8_t prev_enpassant_target;
  uint8_t prev_half_move_clock;
  uint8_t prev_full_move_number;
} MoveHistoryEntry;

Move encode_move(Piece piece, uint8_t from_sq, uint8_t to_sq,
                 Piece captured_piece, Piece promoted_piece, bool flag_ps,
                 bool flag_ep, uint32_t flag_ca);

Move encode_quite_move(Piece piece, uint8_t from_sq, uint8_t to_sq);

Move encode_capture_move(Piece piece, uint8_t from_sq, uint8_t to_sq,
                         Piece captured_piece);

Move encode_pawn_start_move(Piece pawn, uint8_t from_sq, uint8_t to_sq);

Move encode_pawn_promotion_move(Piece pawn, uint8_t from_sq, uint8_t to_sq,
                                Piece captured_piece, Piece promoted_piece);

Move encode_pawn_enpassant_move(Piece pawn, uint8_t from_sq, uint8_t to_sq,
                                Piece captured_pawn);

Move encode_king_castle_move(Piece piece, uint8_t from_sq, uint8_t to_sq,
                             uint32_t flag_ca);

void do_move(Position *position, Move move);

void undo_move(Position *position, Move move);

void print_move(Move move);

#endif
