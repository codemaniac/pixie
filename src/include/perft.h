#ifndef PERFT_H
#define PERFT_H

#include "position.h"
#include "threadpool.h"
#include <cstdint>

uint64_t perft_multithreaded(Position& position, const uint8_t depth, ThreadPool& pool);

uint64_t divide(Position& position, const uint8_t depth);

#endif
