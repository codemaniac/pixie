#ifndef FEN_H
#define FEN_H

#include "position.h"

#define MAX_FEN_SIZE 100

Position *fen_to_position(char *fen);
char *position_to_fen(Position *position);

#endif
