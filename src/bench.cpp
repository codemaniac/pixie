#include "include/bench.h"
#include "include/fen.h"
#include "include/position.h"
#include "include/search.h"
#include "include/tt.h"
#include "include/utils.h"
#include <cstdint>
#include <iostream>
#include <memory>

void bench() {
    position_init();

    std::unique_ptr<Position>           position;
    std::unique_ptr<TranspositionTable> table = std::make_unique<TranspositionTable>(8);

    const std::array<std::string, 5> fen_array = {
      "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 2",
      "rnbqkbnr/pp1ppppp/2p5/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 2",
      "rnbqkb1r/pppppppp/5n2/8/3P4/8/PPP1PPPP/RNBQKBNR w KQkq - 1 2",
      "rnbqkbnr/pp1ppppp/8/2p5/2P5/8/PP1PPPPP/RNBQKBNR w KQkq - 0 2",
      "rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 2"};

    uint64_t total_nodes = 0ULL;

    const uint64_t starttime = utils_get_current_time_in_milliseconds();
    for (const std::string& fen : fen_array)
    {
        position = std::make_unique<Position>();
        fen_to_position(fen, position);

        SearchInfo info;
        info.depth         = 6;
        info.timeset       = false;
        info.starttime     = utils_get_current_time_in_milliseconds();
        info.stoptime      = 0;
        info.stopped       = false;
        info.nodes         = 0ULL;
        info.use_iterative = true;
        info.use_uci       = false;

        (void) search(position, table, &info);

        total_nodes += info.nodes;
    }
    const uint64_t stoptime = utils_get_current_time_in_milliseconds();
    const uint64_t time     = stoptime - starttime;

    std::cout << (unsigned long long) total_nodes << " nodes " << (total_nodes * 1000) / time
              << " nps" << std::endl;
}
