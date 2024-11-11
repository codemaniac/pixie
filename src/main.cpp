#include "include/perft.h"
#include "include/position.h"
#include "include/uci.h"
#include "include/utils.h"
#include <cstdint>
#include <iostream>
#include <memory>

void bench() {
    position_init();
    std::unique_ptr<Position> position = std::make_unique<Position>();
    position->set_start_pos();
    const uint64_t starttime   = utils_get_current_time_in_milliseconds();
    uint64_t       total_nodes = divide(position, 6, false);
    const uint64_t stoptime    = utils_get_current_time_in_milliseconds();
    const uint64_t time        = stoptime - starttime;
    std::cout << (unsigned long long) total_nodes << " nodes " << (total_nodes * 1000) / time
              << " nps" << std::endl;
}

int main(int argc, char* argv[]) {
    // Random number generator seed
    srand(3094875238);

    if (argc == 1)
    {
        uci_loop();
    }
    else if (argc == 2 && std::string(argv[1]) == "bench")
    {
        bench();
    }
    return 0;
}
