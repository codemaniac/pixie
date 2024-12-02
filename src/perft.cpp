#include "include/perft.h"
#include "include/containers.h"
#include "include/move.h"
#include "include/position.h"
#include "include/threadpool.h"
#include <cstdint>
#include <future>
#include <iostream>
#include <vector>

static uint64_t perft(Position& position, const uint8_t depth) {
    ArrayList<Move> candidate_moves;
    uint64_t        nodes = 0ULL;

    if (depth == 0)
        return 1ULL;

    position.generate_pseudolegal_moves(&candidate_moves, false);

    for (const Move& move : candidate_moves)
    {
        if (position.move_do(move))
            nodes += perft(position, depth - 1);
        position.move_undo();
    }

    return nodes;
}

uint64_t divide(Position& position, const uint8_t depth) {

    if (depth == 0)
        return 1ULL;

    ArrayList<Move> candidate_moves;
    position.generate_pseudolegal_moves(&candidate_moves, false);

    uint64_t nodes       = 0ULL;
    uint64_t total_nodes = 0ULL;

    for (const Move& move : candidate_moves)
    {
        if (position.move_do(move))
        {
            nodes = perft(position, depth - 1);
            total_nodes += nodes;
            move.display();
            std::cout << " " << (unsigned long long) nodes << std::endl;
        }
        position.move_undo();
    }

    std::cout << std::endl << "Perft = " << (unsigned long long) total_nodes << std::endl;

    return total_nodes;
}

uint64_t perft_multithreaded(Position& position, const uint8_t depth, ThreadPool& pool) {

    if (depth == 0)
        return 1ULL;

    std::vector<std::future<uint64_t>> futures;

    ArrayList<Move> candidate_moves;
    uint64_t        total_nodes = 0ULL;

    position.generate_pseudolegal_moves(&candidate_moves, false);

    for (const Move& move : candidate_moves)
    {
        futures.push_back(pool.enqueue([&position, &move, depth] {
            uint64_t nodes          = 0ULL;
            Position position_clone = Position(position);
            if (position_clone.move_do(move))
                nodes = perft(position_clone, depth - 1);
            return nodes;
        }));
    }

    for (auto& future : futures)
        total_nodes += future.get();

    return total_nodes;
}
