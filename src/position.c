#include "include/position.h"
#include "include/piece.h"
#include <stdbool.h>

bool is_square_attacked(Position *position, uint8_t sq) {
  uint8_t orientation = (position->active_color == WHITE) ? 1 : -1;
  u_int8_t attack_sq;

  // Check pawn attacks
  u_int8_t pawn_attack_rays[] = {9, 11};
  for (uint8_t i = 0; i < 2; i++) {
    attack_sq = sq + pawn_attack_rays[i] * orientation;
    if (POS_IS_SQ_ON_BOARD(attack_sq) &&
        !POS_IS_SQ_INVALID(position, attack_sq) &&
        PIECE_GET_TYPE(POS_PIECE(position, attack_sq)) == PAWN &&
        PIECE_GET_COLOR(POS_PIECE(position, attack_sq)) !=
            POS_ACTIVE_COLOR(position)) {
      return true;
    }
  }

  // TODO: Check en passant pawn attacks

  // Check knight attacks
  for (uint8_t i = 0; i < 8; i++) {
    attack_sq = sq + KNIGHT_MOVE_DIRECTIONS[i];
    if (POS_IS_SQ_ON_BOARD(attack_sq) &&
        !POS_IS_SQ_INVALID(position, attack_sq) &&
        PIECE_GET_TYPE(POS_PIECE(position, attack_sq)) == KNIGHT &&
        PIECE_GET_COLOR(POS_PIECE(position, attack_sq)) !=
            POS_ACTIVE_COLOR(position)) {
      return true;
    }
  }

  // Check rook and queen attacks
  for (uint8_t i = 0; i < 4; i++) {
    attack_sq = sq + ROOK_MOVE_DIRECTIONS[i];
    while (POS_IS_SQ_ON_BOARD(attack_sq) &&
           !POS_IS_SQ_INVALID(position, attack_sq)) {
      if (!POS_IS_SQ_EMPTY(position, attack_sq)) {
        if ((PIECE_GET_TYPE(POS_PIECE(position, attack_sq)) == ROOK ||
             PIECE_GET_TYPE(POS_PIECE(position, attack_sq)) == QUEEN) &&
            PIECE_GET_COLOR(POS_PIECE(position, attack_sq)) !=
                POS_ACTIVE_COLOR(position)) {
          return true;
        }
        break;
      }
      attack_sq = attack_sq + ROOK_MOVE_DIRECTIONS[i];
    }
  }

  // Check bishop and queen attacks
  for (uint8_t i = 0; i < 4; i++) {
    attack_sq = sq + BISHOP_MOVE_DIRECTIONS[i];
    while (POS_IS_SQ_ON_BOARD(attack_sq) &&
           !POS_IS_SQ_INVALID(position, attack_sq)) {
      if (!POS_IS_SQ_EMPTY(position, attack_sq)) {
        if ((PIECE_GET_TYPE(POS_PIECE(position, attack_sq)) == BISHOP ||
             PIECE_GET_TYPE(POS_PIECE(position, attack_sq)) == QUEEN) &&
            PIECE_GET_COLOR(POS_PIECE(position, attack_sq)) !=
                POS_ACTIVE_COLOR(position)) {
          return true;
        }
        break;
      }
      attack_sq = attack_sq + BISHOP_MOVE_DIRECTIONS[i];
    }
  }

  // Check king attacks
  for (uint8_t i = 0; i < 8; i++) {
    attack_sq = sq + KING_MOVE_DIRECTIONS[i];
    if (POS_IS_SQ_ON_BOARD(attack_sq) &&
        !POS_IS_SQ_INVALID(position, attack_sq) &&
        PIECE_GET_TYPE(POS_PIECE(position, attack_sq)) == KING &&
        PIECE_GET_COLOR(POS_PIECE(position, attack_sq)) !=
            POS_ACTIVE_COLOR(position)) {
      return true;
    }
  }

  return false;
}

bool is_position_in_check(Position *position) {
  if (position->active_color == WHITE) {
    return is_square_attacked(position, position->board->wK_sq);
  } else {
    return is_square_attacked(position, position->board->bK_sq);
  }
}
