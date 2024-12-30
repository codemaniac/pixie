#include "engine.h"

namespace tejas {

    Engine::Engine() {}

    Engine::~Engine() {}

    void Engine::set_startpos() { board.set_startpos(); }

#ifdef DEBUG
    void Engine::display_board() { board.display(); }
#endif

}
