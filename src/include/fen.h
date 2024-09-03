#ifndef FEN_H
#define FEN_H

#include "chess.h"

#define MAX_FEN_SIZE 100

Position* fen_to_position(const char* fen);

#endif
