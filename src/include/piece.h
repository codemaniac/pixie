#ifndef PIECE_H
#define PIECE_H

#include <stdint.h>

#define PIECE_GET_TYPE(id) (id & 0x7)
#define PIECE_GET_COLOR(id) ((id & 0x8) / 8)
#define PIECE_CREATE(t, c) ((c << 3) | t)

typedef enum { WHITE = 0, BLACK } Color;

typedef enum { PAWN = 1, KNIGHT, BISHOP, ROOK, QUEEN, KING } PieceType;

typedef enum {
  NO_PIECE = 0,
  WHITE_PAWN,
  WHITE_KNIGHT,
  WHITE_BISHOP,
  WHITE_ROOK,
  WHITE_QUEEN,
  WHITE_KING,
  BLACK_PAWN = 9,
  BLACK_KNIGHT,
  BLACK_BISHOP,
  BLACK_ROOK,
  BLACK_QUEEN,
  BLACK_KING
} Piece;

enum { NOCA = 0, WKCA = 1, WQCA = 2, BKCA = 4, BQCA = 8 };

static const uint8_t PAWN_CAPTURE_DIRECTIONS[] = {9, 11};
static const int8_t KNIGHT_MOVE_DIRECTIONS[] = {-21, -19, -12, -8,
                                                8,   12,  19,  21};
static const int8_t BISHOP_MOVE_DIRECTIONS[] = {-11, -9, 9, 11};
static const int8_t ROOK_MOVE_DIRECTIONS[] = {-10, -1, 1, 10};
static const int8_t QUEEN_MOVE_DIRECTIONS[] = {-11, -10, -9, -1, 1, 9, 10, 11};
static const int8_t KING_MOVE_DIRECTIONS[] = {-11, -10, -9, -1, 1, 9, 10, 11};

#endif
