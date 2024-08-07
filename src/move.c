#include "include/move.h"
#include "include/piece.h"
#include "include/position.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

Move encode_move(Piece piece, uint8_t from_sq, uint8_t to_sq,
                 Piece captured_piece, Piece promoted_piece, bool flag_ps,
                 bool flag_ep, uint32_t flag_ca) {
  uint32_t move_id = (piece) | (from_sq << 4) | (to_sq << 11) |
                     (captured_piece << 18) | (promoted_piece << 23);
  if (flag_ps) {
    move_id |= MOVE_FLAG_PS;
  } else if (flag_ep) {
    move_id |= MOVE_FLAG_EP;
  } else if (flag_ca) {
    move_id |= flag_ca;
  }

  Move move = {.move_id = move_id};
  return move;
}

Move encode_quite_move(Piece piece, uint8_t from_sq, uint8_t to_sq) {
  return encode_move(piece, from_sq, to_sq, NO_PIECE, NO_PIECE, false, false,
                     0);
}

Move encode_capture_move(Piece piece, uint8_t from_sq, uint8_t to_sq,
                         Piece captured_piece) {
  return encode_move(piece, from_sq, to_sq, captured_piece, NO_PIECE, false,
                     false, 0);
}

Move encode_pawn_start_move(Piece pawn, uint8_t from_sq, uint8_t to_sq) {
  return encode_move(pawn, from_sq, to_sq, NO_PIECE, NO_PIECE, true, false, 0);
}

Move encode_pawn_promotion_move(Piece pawn, uint8_t from_sq, uint8_t to_sq,
                                Piece captured_piece, Piece promoted_piece) {
  return encode_move(pawn, from_sq, to_sq, captured_piece, promoted_piece,
                     false, false, 0);
}

Move encode_pawn_enpassant_move(Piece pawn, uint8_t from_sq, uint8_t to_sq,
                                Piece captured_pawn) {
  return encode_move(pawn, from_sq, to_sq, captured_pawn, NO_PIECE, false, true,
                     0);
}

Move encode_king_castle_move(Piece king, uint8_t from_sq, uint8_t to_sq,
                             uint32_t flag_ca) {
  return encode_move(king, from_sq, to_sq, NO_PIECE, NO_PIECE, false, false,
                     flag_ca);
}

void do_move(Position *position, Move move) {

  POS_SET_PIECE(position, MOVE_FROM_SQ(move.move_id), SQUARE_EMPTY);

  if (MOVE_PROMOTED(move.move_id)) {
    // Promotion move
    POS_SET_PIECE(position, MOVE_TO_SQ(move.move_id),
                  MOVE_PROMOTED(move.move_id));
  } else if (move.move_id & MOVE_FLAG_EP) {
    // En Passant move
    POS_SET_PIECE(position, MOVE_TO_SQ(move.move_id), MOVE_PIECE(move.move_id));
    // Remove EP captured pawn
    int8_t orientation = (POS_ACTIVE_COLOR(position) == WHITE) ? 1 : -1;
    POS_SET_PIECE(position, MOVE_TO_SQ(move.move_id) - 10 * orientation,
                  SQUARE_EMPTY);
  } else if (move.move_id & MOVE_FLAG_WKCA) {
    // WKCA
    // TODO: Check castle eligibility
    POS_SET_PIECE(position, POS_RF_TO_SQ(RANK_1, FILE_E), SQUARE_EMPTY);
    POS_SET_PIECE(position, POS_RF_TO_SQ(RANK_1, FILE_F), WHITE_ROOK);
    POS_SET_PIECE(position, POS_RF_TO_SQ(RANK_1, FILE_G), WHITE_KING);
    POS_SET_PIECE(position, POS_RF_TO_SQ(RANK_1, FILE_H), SQUARE_EMPTY);
    position->casteling_rights ^= WKCA;
    position->casteling_rights ^= WQCA;
  } else if (move.move_id & MOVE_FLAG_WQCA) {
    // WQCA
    // TODO: Check castle eligibility
    POS_SET_PIECE(position, POS_RF_TO_SQ(RANK_1, FILE_E), SQUARE_EMPTY);
    POS_SET_PIECE(position, POS_RF_TO_SQ(RANK_1, FILE_D), WHITE_ROOK);
    POS_SET_PIECE(position, POS_RF_TO_SQ(RANK_1, FILE_C), WHITE_KING);
    POS_SET_PIECE(position, POS_RF_TO_SQ(RANK_1, FILE_B), SQUARE_EMPTY);
    POS_SET_PIECE(position, POS_RF_TO_SQ(RANK_1, FILE_A), SQUARE_EMPTY);
    position->casteling_rights ^= WKCA;
    position->casteling_rights ^= WQCA;
  } else if (move.move_id & MOVE_FLAG_BKCA) {
    // BKCA
    // TODO: Check castle eligibility
    POS_SET_PIECE(position, POS_RF_TO_SQ(RANK_8, FILE_E), SQUARE_EMPTY);
    POS_SET_PIECE(position, POS_RF_TO_SQ(RANK_8, FILE_F), WHITE_ROOK);
    POS_SET_PIECE(position, POS_RF_TO_SQ(RANK_8, FILE_G), WHITE_KING);
    POS_SET_PIECE(position, POS_RF_TO_SQ(RANK_8, FILE_H), SQUARE_EMPTY);
    position->casteling_rights ^= BKCA;
    position->casteling_rights ^= BQCA;
  } else if (move.move_id & MOVE_FLAG_BQCA) {
    // BQCA
    // TODO: Check castle eligibility
    POS_SET_PIECE(position, POS_RF_TO_SQ(RANK_8, FILE_E), SQUARE_EMPTY);
    POS_SET_PIECE(position, POS_RF_TO_SQ(RANK_8, FILE_D), WHITE_ROOK);
    POS_SET_PIECE(position, POS_RF_TO_SQ(RANK_8, FILE_C), WHITE_KING);
    POS_SET_PIECE(position, POS_RF_TO_SQ(RANK_8, FILE_B), SQUARE_EMPTY);
    POS_SET_PIECE(position, POS_RF_TO_SQ(RANK_8, FILE_A), SQUARE_EMPTY);
    position->casteling_rights ^= BKCA;
    position->casteling_rights ^= BQCA;
  } else {
    // Quite or capture move
    POS_SET_PIECE(position, MOVE_TO_SQ(move.move_id), MOVE_PIECE(move.move_id));

    // Handle pawn start move
    if (move.move_id & MOVE_FLAG_PS) {
      // Set En passant target square in position
      int8_t orientation = (POS_ACTIVE_COLOR(position) == WHITE) ? 1 : -1;
      position->enpassant_target = MOVE_TO_SQ(move.move_id) - 10 * orientation;
    }
  }
}

void print_move(const Move move) {
  const char *pieces = ".PNBRQK";
  const char *files = "abcdefgh";

  const char piece = pieces[(MOVE_PIECE(move.move_id) & 0x7)];

  const int src_rank = POS_SQ_TO_RANK(MOVE_FROM_SQ(move.move_id)) + 1;
  const char src_file = files[POS_SQ_TO_FILE(MOVE_FROM_SQ(move.move_id))];

  const int dest_rank = POS_SQ_TO_RANK(MOVE_TO_SQ(move.move_id)) + 1;
  const char dest_file = files[POS_SQ_TO_FILE(MOVE_TO_SQ(move.move_id))];

  printf("%c%c%d%c%d\n", piece, src_file, src_rank, dest_file, dest_rank);
}
