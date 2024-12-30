#include "engine.h"
#include "movegen.h"

namespace tejas {

    Engine::Engine() { movegen::initialize(); }

    Engine::~Engine() {}

    void Engine::setStartpos() { board.setStartpos(); }

#ifdef DEBUG
    void Engine::displayBoard() { board.display(); }
#endif

}
