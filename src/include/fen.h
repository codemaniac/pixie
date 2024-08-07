#ifndef FEN_H
#define FEN_H

#include "position.h"

#define MAX_FEN_SIZE 100

Position* fen_to_position(const char* fen);
char*     position_to_fen(const Position* position);

#endif
