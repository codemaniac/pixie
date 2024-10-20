#include "include/perft.h"
#include "include/containers.h"
#include "include/position.h"
#include "include/threadpool.h"
#include <cstdint>
#include <future>
#include <memory>
#include <vector>

uint64_t perft(std::unique_ptr<Position>& position, const uint8_t depth) {
    ArrayList<Move> candidate_moves;
    uint64_t        nodes = 0ULL;

    if (depth == 0)
        return 1ULL;

    position->generate_pseudolegal_moves(&candidate_moves, false);

    for (const Move& move : candidate_moves)
    {
        if (position->move_do(move))
            nodes += perft(position, depth - 1);
        position->move_undo();
    }

    return nodes;
}

uint64_t perft_multithreaded(std::unique_ptr<Position>&   position,
                             const uint8_t                depth,
                             std::unique_ptr<ThreadPool>& pool) {

    std::vector<std::future<uint64_t>> futures;

    ArrayList<Move> candidate_moves;
    uint64_t        total_nodes = 0ULL;

    position->generate_pseudolegal_moves(&candidate_moves, false);

    for (const Move& move : candidate_moves)
    {
        futures.push_back(pool->enqueue([&position, &move, depth] {
            uint64_t                  nodes          = 0ULL;
            std::unique_ptr<Position> position_clone = std::make_unique<Position>(*position.get());
            if (position_clone->move_do(move))
                nodes = perft(position_clone, depth - 1);
            return nodes;
        }));
    }

    for (auto& future : futures)
        total_nodes += future.get();

    return total_nodes;
}