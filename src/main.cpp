#include "include/bench.h"
#include "include/uci.h"
#include <string>

int main(int argc, char* argv[]) {
    // Random number generator seed
    srand(3094875238);

    if (argc == 1)
        uci_loop();
    else if (argc == 2 && std::string(argv[1]) == "bench")
        bench();
    return 0;
}
