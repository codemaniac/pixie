#ifndef PERFT_H
#define PERFT_H

#include "position.h"
#include "threadpool.h"
#include <cstdint>

uint64_t perft(std::unique_ptr<Position>&, const uint8_t depth);
uint64_t perft_multithreaded(std::unique_ptr<Position>&,
                             const uint8_t                depth,
                             std::unique_ptr<ThreadPool>& pool);

#endif