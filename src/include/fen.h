#ifndef FEN_H
#define FEN_H

#include "position.h"

void fen_to_position(std::string fen, std::unique_ptr<Position>& pos);

#endif