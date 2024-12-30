#include "engine.h"

int main() {
    tejas::Engine engine;
    engine.set_startpos();
#ifdef DEBUG
    engine.display_board();
#endif
    return 0;
}
