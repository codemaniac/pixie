#include "engine.h"

int main() {
    tejas::Engine engine;
    engine.SetStartpos();
#ifdef DEBUG
    engine.DisplayBoard();
#endif
    return 0;
}
