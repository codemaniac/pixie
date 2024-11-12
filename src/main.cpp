#include "include/bench.h"
#include "include/uci.h"
#include <string>

int main(int argc, char* argv[]) {
    if (argc == 1)
        uci_loop();
    else if (argc == 2 && std::string(argv[1]) == "bench")
        bench();
    return 0;
}
