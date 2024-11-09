#ifndef EVAL_H
#define EVAL_H

#include "position.h"
#include <cstdint>
#include <memory>

void    eval_init();
int32_t eval_position(std::unique_ptr<Position>& position);
bool    eval_is_end_game(std::unique_ptr<Position>& position);

#endif
