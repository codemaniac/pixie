#ifndef POSITION_H
#define POSITION_H

#include "board.h"
#include "piece.h"
#include <stdbool.h>
#include <stdint.h>

#define SQUARE_INVALID __SQUARE_INVALID
#define SQUARE_EMPTY __SQUARE_EMPTY
#define POS_IS_SQ_ON_BOARD(sq) (__IS_SQ_ON_BOARD(sq))
#define POS_RF_TO_SQ(r, f) (__RF_TO_SQ(r, f))
#define POS_SQ_TO_RANK(sq) (__SQ_TO_RANK(sq))
#define POS_SQ_TO_FILE(sq) (__SQ_TO_FILE(sq))
#define POS_IS_SQ_INVALID(pos, sq) (pos->board->squares[sq] == SQUARE_INVALID)
#define POS_IS_SQ_EMPTY(pos, sq) (pos->board->squares[sq] == SQUARE_EMPTY)
#define POS_ACTIVE_COLOR(pos) (pos->active_color)
#define POS_PIECE(pos, sq) (pos->board->squares[sq])
#define POS_SET_PIECE(pos, sq, p) (pos->board->squares[sq] = p)

typedef enum {
  RANK_1,
  RANK_2,
  RANK_3,
  RANK_4,
  RANK_5,
  RANK_6,
  RANK_7,
  RANK_8
} Rank;

typedef enum {
  FILE_A,
  FILE_B,
  FILE_C,
  FILE_D,
  FILE_E,
  FILE_F,
  FILE_G,
  FILE_H
} File;

typedef struct {
  Board *board;
  Color active_color;
  uint8_t casteling_rights;
  uint8_t enpassant_target;
  uint8_t half_move_clock;
  uint8_t full_move_number;
} Position;

bool is_square_attacked(Position *position, uint8_t sq);

bool is_position_in_check(Position *position);

#endif
