#include "include/bench.h"
#include "include/tune.h"
#include "include/uci.h"
#include <string>

int main(int argc, char* argv[]) {
    if (argc == 1)
    {
        uci_loop();
    }
    else if (argc == 2)
    {
        if (std::string(argv[1]) == "bench")
        {
            bench();
        }
        else if (std::string(argv[1]) == "tune")
        {
            tune();
        }
    }
    return 0;
}
