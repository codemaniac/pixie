#include "engine.h"

int main() {
    tejas::Engine engine;
    engine.setStartpos();
#ifdef DEBUG
    engine.displayBoard();
#endif
    return 0;
}
