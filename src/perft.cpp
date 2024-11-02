#include "include/perft.h"
#include "include/containers.h"
#include "include/move.h"
#include "include/position.h"
#include "include/threadpool.h"
#include <cstdint>
#include <future>
#include <memory>
#include <vector>

static uint64_t _perft(std::unique_ptr<Position>& position,
                       const Move&                move,
                       const uint8_t              depth,
                       const bool                 captures_only) {
    ArrayList<Move> candidate_moves;
    uint64_t        nodes = 0ULL;

    if (depth == 0)
    {
        if (captures_only)
        {
            if (MOVE_IS_CAPTURE(move.get_flag()))
                return 1ULL;
            else
                return 0ULL;
        }
        else
            return 1ULL;
    }

    position->generate_pseudolegal_moves(&candidate_moves, false);

    for (const Move& move : candidate_moves)
    {
        if (position->move_do(move))
            nodes += _perft(position, move, depth - 1, captures_only);
        position->move_undo();
    }

    return nodes;
}

uint64_t perft_multithreaded(std::unique_ptr<Position>&   position,
                             const uint8_t                depth,
                             std::unique_ptr<ThreadPool>& pool,
                             const bool                   captures_only) {

    std::vector<std::future<uint64_t>> futures;

    ArrayList<Move> candidate_moves;
    uint64_t        total_nodes = 0ULL;

    position->generate_pseudolegal_moves(&candidate_moves, false);

    for (const Move& move : candidate_moves)
    {
        futures.push_back(pool->enqueue([&position, &move, depth, captures_only] {
            uint64_t                  nodes          = 0ULL;
            std::unique_ptr<Position> position_clone = std::make_unique<Position>(*position.get());
            if (position_clone->move_do(move))
                nodes = _perft(position_clone, move, depth - 1, captures_only);
            return nodes;
        }));
    }

    for (auto& future : futures)
        total_nodes += future.get();

    return total_nodes;
}
