#include "engine.h"
#include "uci.h"

int main() {
    tejas::Engine          engine;
    tejas::uci::UCIHandler ucihandler(engine);
    ucihandler.start();
    return 0;
}
