#include "engine.h"
#include "movegen.h"

namespace tejas {

    Engine::Engine() {
        board::Board::initialize();
        movegen::initialize();
    }

    Engine::~Engine() {}

    void Engine::setStartpos() { board.setStartpos(); }

#ifdef DEBUG
    void Engine::displayBoard() { board.display(); }
#endif

}
