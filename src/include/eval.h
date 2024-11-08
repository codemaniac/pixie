#ifndef EVAL_H
#define EVAL_H

#include "position.h"
#include <cstdint>
#include <memory>

int32_t eval_position(std::unique_ptr<Position>& position);
bool    eval_has_big_pieces(std::unique_ptr<Position>& position);

#endif
