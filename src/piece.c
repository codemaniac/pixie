#include "include/piece.h"

const uint8_t PAWN_CAPTURE_DIRECTIONS[] = {9, 11};
const int8_t  KNIGHT_MOVE_DIRECTIONS[]  = {-21, -19, -12, -8, 8, 12, 19, 21};
const int8_t  BISHOP_MOVE_DIRECTIONS[]  = {-11, -9, 9, 11};
const int8_t  ROOK_MOVE_DIRECTIONS[]    = {-10, -1, 1, 10};
const int8_t  QUEEN_MOVE_DIRECTIONS[]   = {-11, -10, -9, -1, 1, 9, 10, 11};
const int8_t  KING_MOVE_DIRECTIONS[]    = {-11, -10, -9, -1, 1, 9, 10, 11};
