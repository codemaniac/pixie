#ifndef FEN_H
#define FEN_H

#include "position.h"

void fen_to_position(std::string fen, Position* pos, const bool full = true);

#endif
