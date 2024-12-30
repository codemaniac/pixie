#ifndef EVAL_H
#define EVAL_H

#include "position.h"
#include <cstdint>

void    eval_init();
int32_t eval_position(Position& position);
bool    eval_is_end_game(Position& position);

#endif
