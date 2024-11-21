#ifndef EVAL_H
#define EVAL_H

#include "position.h"
#include <cstdint>

int32_t eval_position(Position& position);
bool    eval_is_end_game(Position& position);

#endif
